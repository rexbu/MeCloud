//
//  base.cpp
//  MeIM
//
//  Created by Fantasist on 14-3-7.
//  Copyright (c) 2014年 Fantasist. All rights reserved.
//

#include "p2p.h"
#include "Push.h"
#ifdef __STUN_SERVER__
#include "redis_db.h"
#endif
#ifdef __ANDROID__
#include <jni.h>
#endif

static void stun_handle(const struct sockaddr_in* addr, char* buf, uint32_t size);
static void push_handle(push::message_t* msg);

static void handle_heart(const struct sockaddr_in* addr, p2p::message_t* msg);
static void handle_nat(p2p::message_t* msg);
static void handle_recv_file(p2p::message_t* msg);
static void handle_send_file(p2p::message_t* msg);
static void handle_hole(const struct sockaddr_in* addr, p2p::message_t* msg);
static void handle_scan(const struct sockaddr_in* addr, p2p::message_t* msg);

static UTPSocket*           utp = NULL;

uint16_t P2P::P2P_STORAGE_EXPIRE = 30;
uint16_t P2P::P2P_HEART_FREQ = 30;
uint16_t P2P::P2P_STUN_EXPIRE = 5;
P2P* P2P::m_instance      = NULL;

//#define __LOCAL_TEST__
P2P::P2P(int sock, int local_port, const char* stun_host, int stun_port, int http_port)
{
    m_sock = sock;
    m_local_port = local_port;
    m_http_port = http_port;
    m_flag_pc = false;
#ifdef __LOCAL_TEST__
    bs_sock_addr(&m_stun_master, server_ip, server_port);
    bs_sock_addr(&m_stun_slave, server_ip, server_port);
#else
    bs_strcpy(m_host, URL_SIZE, stun_host);
    char    stun[64];
    sprintf(stun, "stun1.%s", stun_host);
    bs_sock_addr(&m_stun_master, stun, stun_port);
    sprintf(stun, "stun2.%s", stun_host);
    bs_sock_addr(&m_stun_slave, stun, stun_port);
#endif
    m_stun_latest = 0;
    memset(m_self, 0, sizeof(m_self));
    memset(&m_local_addr, 0, sizeof(struct sockaddr_in));
    memset(&m_nat_addr, 0, sizeof(struct sockaddr_in));
    m_flag_cone = false;
    m_conn_status = p2p::STATUS_NULL;
    m_work_status = p2p::STATUS_NULL;
    bs_strcpy(m_tmp_path, URL_SIZE, "./");
    
#ifdef __debug
    bs_log_init("stdout");
    bs_log_set(g_log, LOG_DEBUG, 1);
    bs_log_set(g_log, LOG_NOTICE, 1);
    bs_log_set(g_log, LOG_ERR, 1);
    bs_log_set(g_log, LOG_WARNING, 1);
#endif
    
    utp = UTPSocket::instance();
    
    utp->setHandle(UTP_TYPE_STUN, stun_handle);
    Push::instance()->setHandle(push::ACTION_P2P, push_handle);
}

void push_handle(push::message_t* push_msg){
    p2p::message_t*      msg = (p2p::message_t*)push_msg->params;
    
    debug_log("push p2p message. action[%s]", p2p::action_name[msg->action]);
    switch (msg->action) {
        case p2p::ACTION_NAT:
            handle_nat(msg);
            break;
        case p2p::ACTION_BROADCAST:
            break;
        case p2p::ACTION_RECV_FILE:
            handle_recv_file(msg);
            break;
        case p2p::ACTION_SEND_FILE:
            handle_send_file(msg);
            break;
        default:
            break;
    }
}

state_t P2P::connect(const char* to){
    char                url[URL_SIZE];
    char                buffer[1024];
    http_response_t     res;
    int                 size;
    
    p2p::user_t*         user = getUser(to);
    if(user!=NULL && time(0)-user->flush<2*P2P_HEART_FREQ){
        if (user->conn == p2p::CONN_LAN) {
            return BS_SUCCESS;
        }
    }
    
    setStatus(p2p::STATUS_WAIT_NAT);
    sprintf(url, "stun.%s:%d/addr?user=%s", m_host, m_http_port, to);
    size = http_get(url, NULL, buffer, sizeof(buffer));
    if (size <= 0) {
        return BS_RECVERR;
    }
    
    http_response_parse(&res, buffer, size);
    if (res.response_code != 200) {
        return BS_NOTFOUND;
    }
    
    P2P*                p2p = P2P::instance();
    UTPSocket*          utp = UTPSocket::instance();
    // 开始4个字节代表查找结果
    state_t             st = *(state_t*)res.body;
    struct sockaddr_in* addr = (sockaddr_in*)((char*)res.body + sizeof(state_t));
    p2p::message_t       message;
    
    if (st != BS_SUCCESS) {
        p2p->setStatus(p2p::STATUS_USER_NOTFOUND);
        err_log("get user[%s] addr error[%d]", to, st);
        return st;
    }
    
    p2p->setStatus(p2p::STATUS_WAIT_HOLE);
    p2p::message_init(&message, p2p::ACTION_NAT, p2p->getSelf(), to);
    
    struct sockaddr_in* local_addr = addr;
    struct sockaddr_in* nat_addr = addr+1;
    bool is_lan = same_nat(nat_addr, p2p->getNatAddr(), local_addr, p2p->getLocalAddr());
    
    utp->sendImmediately(nat_addr, &message, p2p::message_size(&message), UTP_TYPE_STUN);
    debug_log("set[%s] nat addr[%s:%d]", to, bs_sock_getip(nat_addr), bs_sock_getport(nat_addr));
    if(is_lan){
        utp->sendImmediately(local_addr, &message, p2p::message_size(&message), UTP_TYPE_STUN);
        debug_log("set[%s] nat addr[%s:%d]", to, bs_sock_getip(local_addr), bs_sock_getport(local_addr));
    }
    
    p2p::message_set(&message, p2p->getLocalAddr(), sizeof(struct sockaddr_in));
    p2p::message_set(&message, p2p->getNatAddr(), sizeof(struct sockaddr_in));
    Push::sendExpanded(to, push::ACTION_P2P, (char*)&message, p2p::message_size(&message));
    
    /*
    // 循环等待返回addr开始打洞
    time_t now = time(0);
    while (getStatus()==p2p::STATUS_WAIT_NAT && time(0)-now<P2P::P2P_STUN_EXPIRE) {}
    // 获取to的addr失败
    if (getStatus()==p2p::STATUS_WAIT_NAT) {
        err_log("get %s addr error. wait timeout", to);
        return BS_TIMEOUT;
    }
    */
    // 循环等待打洞消息
    time_t now = time(0);
    while (getStatus()==p2p::STATUS_WAIT_HOLE && time(0)-now<P2P::P2P_STUN_EXPIRE) {}
    st = getStatus();
    switch (st) {
        case p2p::STATUS_USER_NOTFOUND:
            err_log("connect %s error. user outline", to);
            setStatus(p2p::STATUS_NULL);
            return BS_NOTFOUND;
        case p2p::STATUS_WAIT_HOLE:
            err_log("connect %s error. wait timeout", to);
            setStatus(p2p::STATUS_NULL);
            return BS_TIMEOUT;
        case p2p::STATUS_CONNECTED:
            notice_log("connect [%s] success", to);
            setStatus(p2p::STATUS_NULL);
            return BS_SUCCESS;
        default:
            return BS_INVALID;
    }
    
    return BS_INVALID;
}

void P2P::sendHeart(){
    p2p::message_t       msg;
    
    if(strlen(m_self) <= 0){
    	return;
    }
    //debug_log("%s heart[%s:%d]",m_self, bs_sock_getip(&m_stun_master), bs_sock_getport(&m_stun_master));
    
    p2p::message_init(&msg, p2p::ACTION_HEART, m_self, P2P_STUN_MASTER);
    p2p::message_set(&msg, &m_local_addr, sizeof(struct sockaddr_in));
    UTPSocket::instance()->sendImmediately(&m_stun_master, &msg, p2p::message_size(&msg), UTP_TYPE_STUN);
    bs_strcpy(msg.to, P2P_USER_SIZE, P2P_STUN_SLAVE);
    UTPSocket::instance()->sendImmediately(&m_stun_slave, &msg, p2p::message_size(&msg), UTP_TYPE_STUN);
}

void stun_handle(const struct sockaddr_in* addr, char* buf, uint32_t size){
    p2p::message_t*      msg;
    
    msg = (p2p::message_t*)buf;
    switch (msg->action) {
        case p2p::ACTION_HEART:
            handle_heart(addr, msg);
            break;
        case p2p::ACTION_NAT:
            debug_log("action[%s] from[%s] to[%s]", p2p::action_name[msg->action], msg->from, msg->to);
            //handle_nat(msg);
            break;
        case p2p::ACTION_HOLE:
            debug_log("action[%s] from[%s] to[%s]", p2p::action_name[msg->action], msg->from, msg->to);
            handle_hole(addr, msg);
            break;
        case p2p::ACTION_SCAN:
            handle_scan(addr, msg);
            break;
        default:
            break;
    }
}

void handle_heart(const struct sockaddr_in* addr, p2p::message_t* msg){
    //debug_log("client heart[%s - %s:%d]", msg->from, bs_sock_getip(addr), bs_sock_getport(addr));
    
    if(strcmp(msg->from, P2P_STUN_MASTER) == 0){
        P2P::instance()->setNatAddr((const struct sockaddr_in*)msg->extra);
        //const struct sockaddr_in* nat_master = (const struct sockaddr_in*)msg->extra;
        //debug_log("master [%s:%d]", bs_sock_getip(nat_master), bs_sock_getport(nat_master));
    }
    else if(strcmp(msg->from, P2P_STUN_SLAVE) == 0){
        const struct sockaddr_in* nat_slave = (const struct sockaddr_in*)msg->extra;
        const struct sockaddr_in* nat_master = P2P::instance()->getNatAddr();
        //debug_log("slave [%s:%d]", bs_sock_getip(nat_slave), bs_sock_getport(nat_slave));
        
        if (nat_master->sin_addr.s_addr==nat_slave->sin_addr.s_addr && nat_master->sin_port==nat_slave->sin_port){
            P2P::instance()->setCone(true);
            debug_log("recv heart. cone nat");
        }
        else{
            P2P::instance()->setCone(false);
            debug_log("recv heart. isn't cone nat");
        }
    }
}

void handle_nat(p2p::message_t* msg){
    P2P*         p2p = P2P::instance();
    UTPSocket*          utp = UTPSocket::instance();
    p2p::message_t       hole_msg;
    struct sockaddr_in* nat_addr;
    struct sockaddr_in* local_addr;
    bool                is_lan;     // 是否局域网内

    // 目前只有一种错误，BS_NOTFOUND
    if (msg->state != BS_SUCCESS) {
        p2p->setStatus(p2p::STATUS_ERROR);
        return;
    }
    
    local_addr = (struct sockaddr_in*)msg->extra;
    nat_addr = (struct sockaddr_in*)msg->extra + 1;
    
    is_lan = same_nat(nat_addr, p2p->getNatAddr(), local_addr, p2p->getLocalAddr());
    
    p2p::message_init(&hole_msg, p2p::ACTION_HOLE, msg->to, msg->from);
    // 先发nat，再发本地，这样接收方后受到local，可以覆盖nat
    utp->sendImmediately(nat_addr, &hole_msg, p2p::message_size(&hole_msg), UTP_TYPE_STUN);
    debug_log("set[%s] hole addr[%s:%d]", hole_msg.to, bs_sock_getip(nat_addr), bs_sock_getport(nat_addr));
    if(is_lan){
        utp->sendImmediately(local_addr, &hole_msg, p2p::message_size(&hole_msg), UTP_TYPE_STUN);
        debug_log("set[%s] hole addr[%s:%d]", hole_msg.to, bs_sock_getip(local_addr), bs_sock_getport(local_addr));
    }
}

class RecvThread: public Thread{
public:
    RecvThread(void* para):Thread(para){
        mDynFlag = false;
    }
    RecvThread(void* para, uint32_t size){
        mPara = new char[size];
        memcpy(mPara, para, size);
        mDynFlag = true;
    }
    
    void run(){
        P2P*            p2p = P2P::instance();
        p2p::message_t* msg = (p2p::message_t*)mPara;
        uint32_t        task_id= *(uint32_t*)msg->extra;
        uint32_t        size = *(uint32_t*)((char*)msg->extra + sizeof(uint32_t));
        char*           file = (char*)msg->extra + sizeof(uint32_t)*2;
        char            path[URL_SIZE];
        
        debug_log("recv file[%s] task_id[%u] size[%u]", file, task_id, size);
        snprintf(path, URL_SIZE, "%s/%s", p2p->getTmpPath(), file);
        p2p->recvFile(path, task_id, size);
        if (mDynFlag) {
            delete (char*)mPara;
        }
    }
protected:
    // 标记mPara是否是动态申请
    bool    mDynFlag;
};

void handle_recv_file(p2p::message_t* msg){
    // msg参数需要在RecvThread内部动态创建
    RecvThread      thread(msg, p2p::message_size(msg));
    thread.start();
}

void handle_send_file(p2p::message_t* msg){
    P2P*        p2p = P2P::instance();
    uint32_t    task_id= *(uint32_t*)msg->extra;
    uint32_t    size = *(uint32_t*)((char*)msg->extra + sizeof(uint32_t));
    char*       file = (char*)msg->extra + sizeof(uint32_t)*2;
    char        path[URL_SIZE];
    
    debug_log("recv file[%s] task_id[%u] size[%u]", file, task_id, size);
    snprintf(path, URL_SIZE, "%s/%s", p2p->getTmpPath(), file);
    p2p->sendFile(msg->from, path, task_id);
}

void handle_hole(const struct sockaddr_in* addr, p2p::message_t* msg){
    P2P*                p2p = P2P::instance();
    int                 type = p2p::CONN_NAT;
    //UTPSocket*         utp = UTPSocket::instance();
    
    if(p2p->getStatus() == p2p::STATUS_WAIT_HOLE){
        p2p->setStatus(p2p::STATUS_CONNECTED);
    }
    
    if (same_lan(addr, p2p->getLocalAddr())) {
        type = p2p::CONN_LAN;
    }
    // 连接发起者可能会收到两次hole消息，第二次收到的是局域网消息，且此时候已经不是p2p::STATUS_WAIT_HOLE，直接写入
    p2p::user_t*    user = p2p->getUser(msg->from);
    if (user == NULL) {
        p2p->setUser(msg->from, type, p2p::STATUS_CONNECTED, addr);
    }
    else{
        user->status = p2p::STATUS_CONNECTED;
    }
    debug_log("hole addr[%s:%d]", bs_sock_getip(addr), bs_sock_getport(addr));
}

void handle_scan(const struct sockaddr_in* addr, p2p::message_t* msg){
    P2P*     p2p = P2P::instance();
    
    if (strcmp(msg->from, "pc")==0) {
        p2p->setPC(addr);
        p2p->m_flag_pc = true;
        debug_log("recv pc addr[%s:%d]", bs_sock_getip(addr), bs_sock_getport(addr));
        return;
    }
    
    p2p->setUser(msg->from, p2p::CONN_LAN, p2p::STATUS_CONNECTED, addr);
    debug_log("recv scan addr[%s:%d]", bs_sock_getip(addr), bs_sock_getport(addr));
}

void P2P::scanLan(){
    p2p::message_t       msg;
    char                ip[IP_SIZE];
    
    if (bs_sock_getport(&m_local_addr)==0 || strlen(m_self) <= 0){
        return;
    }

    //bs_sock_localip(ip, sizeof(ip));
    p2p::message_init(&msg, p2p::ACTION_SCAN, m_self, NULL);
    bs_strcpy(ip, IP_SIZE, bs_sock_getip(&m_local_addr));
    
    int i= (int)strlen(ip);
    while (ip[i]!='.') {
        i--;
    }
    
    int ipnum = atoi(ip+i+1);
    for (int startip = 0; startip<256; startip++) {
        // 不发自己
        if (startip==ipnum) {
            continue;
        }

        sprintf(ip+i+1, "%d", startip);
        utp->sendImmediately(ip, utp->getPort(), &msg, p2p::message_size(&msg), UTP_TYPE_STUN);
    }
}

state_t P2P::sendBroadcast(const char* user, const char* buf, uint32_t size){
    p2p::user_t*     puser = getUser(user);
    if (puser == NULL) {
        return BS_NOTFOUND;
    }
    
    return utp->send(&puser->addr, buf, size, UTP_TYPE_BROADCAST);
}

int P2P::recvBroadcast(const char* user, char* buf, uint32_t size){
    UTPSocket*     utp = UTPSocket::instance();
    p2p::user_t*     puser = getUser(user);
    
    if (puser == NULL) {
        return BS_NOTFOUND;
    }
    
    return utp->utp_recv(buf, size);
}

state_t P2P::sendBlock(const char* user, const char* buf, uint32_t size){
    p2p::user_t*     puser = getUser(user);
    return utp->sendBlock(&puser->addr, buf, size);
}

state_t P2P::checkLan(){
    /*
    for(map<string, p2p::user_t>::iterator iter = m_users.begin();
            iter != m_users.end(); iter++){
        p2p::user_t& user = iter->second;
        if(user.type == p2p::CONN_LAN && time(0)-user.flush>= 3*P2P_HEART_FREQ){
            m_users.erase(iter);
        }
    }
     */

    return BS_SUCCESS;
}

state_t P2P::sendFile(const char* to, const char* path, uint32_t task_id){
    char            buf[1024];
    uint32_t        size;
    uint32_t        head_size = 2*sizeof(uint32_t);
    uint32_t        id = 0;
    
    p2p::user_t*    user = getUser(to);
    int             file = open(path, O_RDONLY);

    if (file <= 0 || user==NULL){
        return BS_INVALID;
    }
    
    while((size = (uint32_t)read(file, buf+head_size, 1024-head_size))>0){
        // 前4位位task_id，后4位位包序号
        memcpy(buf, &task_id, sizeof(uint32_t));
        memcpy(buf+sizeof(uint32_t), &id, sizeof(uint32_t));
        state_t st = utp->sendBlock(&user->addr, buf, size+head_size, UTP_TYPE_FILE);
        if (st != BS_SUCCESS) {
            err_log("send file[%s] error[%d]", path, st);
            return st;
        }
        id++;
    }
    close(file);
    return BS_SUCCESS;
}

state_t P2P::recvFile(const char* path, uint32_t task_id, uint32_t size){
    int             file;
    char            buf[10240];
    int             recv_size = 0;
    uint32_t        buf_size = 0;
    struct timeval  inter;
    time_t          now;
    uint32_t        wait_count;

    file = open(path, O_RDWR|O_CREAT);
    if(file<=0){
        return BS_INVALID;
    }

    inter.tv_sec = 0;
    inter.tv_usec = UTPSocket::UTP_SEND_FREQ;
    now = time(0);
    wait_count = 0;
    while(buf_size<size && time(0)-now<UTP_DEF_TIMEOUT){
        if ((recv_size = utp->utp_select()) > 0){
            recv_size = utp->utp_recv(buf, 10240);
            write(file, buf+2*sizeof(uint32_t), recv_size-2*sizeof(uint32_t));
            buf_size += (recv_size-2*sizeof(uint32_t));
            
            wait_count=0;
            now = time(0);
            
            debug_log("buf_size[%d] total[%d] recv_size[%d]", buf_size, size, recv_size-2*(int)sizeof(uint32_t));
        }
        else{
            // 进入休眠等待次数越多休眠时间越长，但最多休眠0.5s
            wait_count++;
            inter.tv_usec = wait_count*UTPSocket::UTP_SEND_FREQ;
            if (inter.tv_usec>500000) {
                inter.tv_usec = 500000;
            }
            bs_timer_sleep(&inter);
        }
    }
    close(file);
    if (buf_size < size) {
        err_log("recv[%s] size[%d/%d] error", path, buf_size, size);
        return BS_TIMEOUT;
    }
    
    debug_log("recv[%s] size[%d/%d] success", path, buf_size, size);
    
    return BS_SUCCESS;
}

void P2P::setUser(const char* user, uint8_t conn, uint8_t status, const struct sockaddr_in* addr){
    p2p::user_t      p2p_user;
    
    p2p_user.conn = conn;
    p2p_user.status = status;
    bs_strcpy(p2p_user.user, P2P_USER_SIZE, user);
    memcpy(&p2p_user.addr, addr, sizeof(struct sockaddr_in));
    p2p_user.flush = time(0);
    
    m_users[string(user)] = p2p_user;
    debug_log("set udp user[%s]", user);
}

void P2P::run(){
	while(BS_TRUE){
        // 心跳
		P2P* p2p = P2P::instance();
		p2p->sendHeart();
        // 扫描
        //p2p->scanLan();
        // 检查通信, 暂时只检查Lan，局域网严格限制正确性，只检查一个周期
        p2p->checkLan();

		sleep(P2P::P2P_HEART_FREQ);
	}
}

