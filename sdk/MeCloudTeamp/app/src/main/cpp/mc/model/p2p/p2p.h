//
//  base.h
//  MeIM
//
//  Created by Fantasist on 14-3-7.
//  Copyright (c) 2014年 Fantasist. All rights reserved.
//

#ifndef __P2P_H_
#define __P2P_H_

#ifdef __cplusplus
#include <iostream>
#include <map>
#include <vector>
#include "bs.h"
#include "basic.h"
#include "utp.h"

using namespace std;

#define __debug(log, args...)   do{   \
char    __buf[BS_DEF_STRLEN];     \
sprintf(__buf, log, ##args);      \
printf("In[%s]: %s\n", __FUNCTION__, __buf);  \
}while(0)

#define P2P_USER_SIZE       25
#define P2P_MESSAGE_SIZE    1024
#define P2P_STUN_MASTER     "stun_master"
#define P2P_STUN_SLAVE      "stun_slave"

// 向Push注册，0-9为push预留
namespace push{
    enum{
        ACTION_P2P = 10
    };
}

namespace p2p {
    enum{
        ACTION_HEART,
        ACTION_NAT,
        ACTION_HOLE,
        ACTION_SCAN,
        ACTION_SCAN_REPLY,
        ACTION_BROADCAST,
        ACTION_RECV_FILE,
        ACTION_SEND_FILE
    };
    
    static const char* action_name[] = {
        "heart", "nat", "hole", "scan", "scan_reply", "broadcast", "recv_file", "send_file"
    };
    
    enum{
        CONN_NAT, /* 非局域网 */ CONN_LAN, /* 局域网内 */
        CONN_NUM
    };
    
    enum{
        STATUS_NULL,
        STATUS_WAIT_NAT,
        STATUS_WAIT_HOLE,
        STATUS_CONNECTED,
        STATUS_USER_NOTFOUND,
        STATUS_ERROR,
        STATUS_NUM
    };
    
    enum{
        STATUS_BROADCAST = 10,
        STATUS_SEND_FILE,
        STATUS_RECV_FILE
    };
    
    typedef struct user_t{
        // 对方和self所处的网络环境，nat或者lan
        uint8_t             conn;
        // 和对方的状态，未连接或者连接
        uint8_t             status;
        char                user[P2P_USER_SIZE];
        sockaddr_in         addr;
        time_t              flush;
    }user_t;
    
    typedef struct message_t{
        int         action:4;
        uint32_t    size:28;
        // 处理结果
        int         state;
        char        from[P2P_USER_SIZE];
        char        to[P2P_USER_SIZE];
        char        extra[P2P_MESSAGE_SIZE];
    }message_t;
    
    inline void message_init(message_t* msg, int act, const char* from, const char* to){
        (msg)->action = act;
        (msg)->size = 0;
        (msg)->state = BS_SUCCESS;
        bs_strcpy((msg)->from, P2P_USER_SIZE, from);
        bs_strcpy((msg)->to, P2P_USER_SIZE, to);
    }
    inline void message_init_status(message_t* msg, int act, int st, const char* from, const char* to){
        message_init(msg, act, from, to);
        msg->state = st;
    }
    
    inline uint32_t message_set(message_t* msg, const void* content, uint32_t size){
        uint32_t rv_size = bs_memcpy(msg->extra+msg->size, P2P_MESSAGE_SIZE-msg->size, content, size);
        msg->size += rv_size;
        return msg->size;
    }
    
    inline uint32_t message_size(message_t* msg){
        return offsetof(message_t, extra) + msg->size;
    }

}

class P2P: public Thread{
public:
    static P2P* initialize(int sock, int local_port, const char* server_ip, int server_port, int http_port){
        if(m_instance==NULL){
            m_instance = new P2P(sock, local_port, server_ip, server_port, http_port);
            m_instance->start();
        }
        return m_instance;
    }
    
    static P2P* instance() {
        return m_instance;
    }
    // 心跳线程
    void run();
    
    p2p::user_t* getUser(const char* user){
        map<string, p2p::user_t>::iterator iter = m_users.find(string(user));
        if (iter == m_users.end()) {
            return NULL;
        }
        return &iter->second;
    }

    void setUser(const char* user, uint8_t conn, uint8_t status, const struct sockaddr_in* addr);
    void setPC(const struct sockaddr_in* addr){
        m_pc = *addr;
    }
    struct sockaddr_in* getPC() {
        if (m_flag_pc) return &m_pc;
        return NULL;
    }
    /*
    void setUser(const char* user, int type, int sock){
        p2p::user_t      p2p_user;
        
        p2p_user.type = type;
        // 只有已经有tcp连接情况下才传入sock
        p2p_user.sock_type = SOCK_STREAM;
        bs_strcpy(p2p_user.user, P2P_USER_SIZE, user);
        p2p_user.sock = sock;
        p2p_user.flush = time(0);
        
        m_users[string(user)] = p2p_user;
        debug_log("set tcp user[%s:%d]", user, sock);
    }
    */
    const char* getLanUser(){
        for (map<string, p2p::user_t>::iterator iter = m_users.begin();
                iter != m_users.end(); iter++){
            if (iter->second.conn == p2p::CONN_LAN && strcmp(iter->second.user, m_self)!=0){
                return iter->second.user;
            }
        }

        return NULL;
    }

    void delUser(const char* user){
        map<string, p2p::user_t>::iterator iter = m_users.find(string(user));
        if (iter != m_users.end()) {
            m_users.erase(iter);
        }
    }

    void sendHeart();
    state_t connect(const char* user);
    state_t sendBlock(const char* user, const char* buf, uint32_t size);
    state_t sendBroadcast(const char* user, const char* buf, uint32_t size);
    int recvBroadcast(const char* user, char* buf, uint32_t size);
    state_t sendFile(const char* user, const char* path, uint32_t task_id);
    state_t recvFile(const char* path, uint32_t task_id, uint32_t size);
    void scanLan();
    state_t checkLan();
    
    void setSelf(const char* user) {
    	bs_strcpy(m_self, P2P_USER_SIZE, user);
    	sendHeart();
    }
    const char* getSelf(){
        return m_self;
    }
    
    int getSocket() { return m_sock;}
    inline bool heartActive() { return time(0)-m_stun_latest < 3*P2P_HEART_FREQ; }
    
    const struct sockaddr_in* getNatAddr(){
        return &m_nat_addr;
    }
    void setNatAddr(const struct sockaddr_in* addr){
        memcpy(&m_nat_addr, addr, sizeof(struct sockaddr_in));
    }
    const struct sockaddr_in* getLocalAddr(){
        return &m_local_addr;
    }
    void setLocalAddr(const char* ip){
        bs_sock_addr(&m_local_addr, ip, m_local_port);
        sendHeart();
    }
    void setLocalAddr(const char* ip, int port){
        bs_sock_addr(&m_local_addr, ip, port);
        m_local_port = port;
    }
    void setLocalAddr(const struct sockaddr_in* addr){
        memcpy(&m_local_addr, addr, sizeof(struct sockaddr_in));
    }
    
    void setCone(bool flag){
        m_flag_cone = flag;
    }
    
    int getStatus(){
        return m_conn_status;
    }
    void setStatus(int status) {
        m_conn_status = status;
    }
    int getWorkStatus() {
        return m_work_status;
    }
    void setWorkStatus(int status){
        m_work_status = status;
    }
    const char* getTmpPath(){
        return m_tmp_path;
    }
    void setTmpPath(const char* path){
        bs_strcpy(m_tmp_path, URL_SIZE, path);
    }
    
    // redis存储自动过期时间
    static uint16_t P2P_STORAGE_EXPIRE;
    static uint16_t P2P_HEART_FREQ;
    // nat请求或者打洞超时时间
    static uint16_t P2P_STUN_EXPIRE;
    
    bool                m_flag_pc;

protected:
    // client端构造函数
    P2P(int sock, int port, const char* server_ip, int server_port, int http_port);
    static P2P*         m_instance;
    
    char                m_self[P2P_USER_SIZE];         // 标示终端用户
    map<string, p2p::user_t> m_users;
    struct sockaddr_in  m_pc;
    
    int                 m_sock;
    char                m_host[URL_SIZE];
    char                m_tmp_path[URL_SIZE];
    time_t				m_stun_latest;	// 最近一次收到stun server的回复消息
    struct sockaddr_in  m_stun_master;
    struct sockaddr_in	m_stun_slave;
    
    int                 m_tcp_sock;
    int                 m_tcp_port;
    int                 m_local_port;
    int                 m_http_port;
    
    struct sockaddr_in  m_local_addr;
    struct sockaddr_in  m_nat_addr;
    // 是否是圆锥型nat
    bool                m_flag_cone;
    // 连接状态
    uint8_t             m_conn_status;
    // 工作状态，空、直播、文件
    uint8_t             m_work_status;
};

// 可能是同一局域网返回bs_true，否则返回bs_false
//bool ip_check_nat(const char* local_ip1, const char* local_ip2, const char* nat_ip1, const char* nat_ip2);
//bool ip_check_address(const char* ip1, const char* ip2);
#endif
#endif /* defined(__MeIM__base__) */
