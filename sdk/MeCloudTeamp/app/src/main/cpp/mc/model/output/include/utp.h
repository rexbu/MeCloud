/**
 * file :	utp.h
 * author :	bushaofeng
 * create :	2014-02-27 20:05
 * func : 多媒体传输协议
 * history:
 */

#ifndef __UTP_H_
#define __UTP_H_

#ifdef __cplusplus

#include <iostream>
#include <map>
#include <vector>
#include "bs.h"
#include "basic.h"
#include "AsyncSocket.h"

using namespace std;

#define UTP_DEF_UDP_PORT        12081
#define UTP_DEF_HTTP_PORT       8088

// 刷新buffer的id间隔
#define UTP_DEF_FLUSH_IDIN      10
#define UTP_DEF_CHECK_UNRECV    10
// 重发未接收到确实时间(单位微秒)
#define UTP_DEF_RESEND_TIMEOUT  100000
// 最多重发次数
#define UTP_DEF_RESEND_NUM      2
// 如果收到重复包，时间间隔太大则刷新缓冲区
#define UTP_DEF_FLUSH_TIMEOUT   10
// 默认发送线程周期
#define UTP_DEF_SEND_FREQ       5000
#define UTP_DEF_TIMEOUT         5
#define UTP_PACK_SIZE    sizeof(_utp_pack_t)

typedef void (* date_func_t)(const struct sockaddr_in* addr, char* buf, uint32_t size);

void utp_send_func(void* para);
void* utp_send_thread(void* session);
void* utp_recv_thread(void* session);
void* utp_handle_thread(void* para);

// 包状态
enum utp_state_t{
    UTP_NULL = 0,
    UTP_STATE_SEND = 1,
    UTP_STATE_RECV,
    UTP_STATE_RESEND,
    UTP_STATE_WAITING,
    UTP_STATE_OK,
    UTP_STATE_ERROR,
    UTP_STATE_NUM
};

enum utp_pos_t{
    UTP_PACK_POS_HEAD = 1,
    UTP_PACK_POS_BODY,
    UTP_PACK_POS_REAR,
    UTP_PACK_POS_NUM
};
enum utp_pack_type_t{
    UTP_PACK_DATA = 0,
    UTP_PACK_SIGNAL
};
enum utp_data_type_t{
    UTP_TYPE_BROADCAST = 1,
    UTP_TYPE_FILE,
    UTP_TYPE_STUN,
    UTP_TYPE_NOTIFY,
    UTP_TYPE_OTHER,
    UTP_TYPE_NUM
};

/* 包头 */
struct utp_head_t{
    uint8_t         sig:4;      // utp_data_type_t类型， 0为数据
    uint8_t         reply:1;    // 是否发确认包
    uint8_t         re_num:3;   // 第几次发送或者申请
    uint8_t         state:4;    // 默认UTP_STATE_SEND，如果重发消息为UTP_STATE_RESEND
    uint8_t         data_type:4; // 数据类型
    uint16_t        pack_size;  // 包长度
    uint16_t        piece_num;  // 总包数
    uint16_t		serial;     // 序号
    uint32_t        identifier;
    uint32_t        check_code;
};

#define UTP_DEF_SLICE_SIZE      (SOCKET_TCP_MTU-sizeof(utp_head_t))
#define UTP_DEF_SLICE_NUM       4096

/* 读写缓存单元 */
struct utp_pack_t{
    utp_head_t      head;
    char            data[UTP_DEF_SLICE_SIZE];
};

struct utp_signal_t{
    uint8_t         sig:4;      // 1为指令
    uint32_t        signal:28;   // 暂时只有请求重发
    uint32_t        para;       // 参数，暂时只为id
};

/* 
 * 包头缓存单元，带包头到数据缓存区的映射
 */
struct utp_elem_t{
    uint8_t         state:4;        // 是否已经收到或者已经发送
    uint8_t         re_num:4;
    uint16_t        piece_num:12;   // 收到的包数
    uint16_t        index:12;       // 缓存队列中位置
    uint32_t        identifier;     // 包id
    struct timeval  flush_time;
};

#define UTP_PACK_ONEPIECE(pack, type, buf, size)      do{  \
    pack.head.sig = UTP_PACK_DATA;      \
    pack.head.reply = 0;                \
    pack.head.data_type = type;         \
    pack.head.state = UTP_STATE_SEND;   \
    pack.head.piece_num = 1;            \
    pack.head.re_num = 0;               \
    pack.head.serial = 0;               \
    pack.head.pack_size = size;         \
    pack.head.piece_num = 1;            \
    pack.head.identifier = 0;           \
    memcpy(pack.data, buf, size);       \
}while(0)
/*
 * 包头队列
 */
class UTPAddrQueue{
public:
    UTPAddrQueue(uint32_t size=UTP_DEF_SLICE_NUM){
        cqueue_init_size(&session, size, BS_FALSE);
        count_id = 0;
    }
    
    UTPAddrQueue(const struct sockaddr_in* addr, uint32_t size=UTP_DEF_SLICE_NUM){
        cqueue_init_size(&session, size, BS_FALSE);
        memcpy(&this->addr, addr, sizeof(struct sockaddr_in));
        count_id = 0;
    }
    
    uint32_t index(uint32_t i){
        return cqueue_index(&session, i);
    }
    utp_elem_t* getNoMatter(uint32_t i){
        return (utp_elem_t*)cqueue_get_any(&session, i);
    }
    void set(uint32_t i, utp_elem_t* elem){
        cqueue_set(&session, i, elem);
    }
    void destroy(){
        cqueue_destroy(&session);
    }
    // 初始化为1， 0用于发送失败
    // 发送方用于标记下个包id，接收方用于存储收到的包id，过滤收到的旧包
    uint32_t                count_id;
    struct sockaddr_in      addr;
    cqueue_t(utp_elem_t)    session;
};

/* 检查包是否就绪 */
struct utp_status_t{
    // 是否就绪可读
    uint8_t                 state:4;
    // addr queue中的位置
    uint32_t                index:28;
    uint32_t                identifier;
    UTPAddrQueue*           addr_queue;
    // 收到的时间或者上次发送重传请求的时间
    struct timeval          flush_time;
};

/**
 * 包头队列、数据缓存区
 */
class UTPIOBase{
public:
    UTPIOBase(uint32_t size=UTP_DEF_SLICE_NUM){
        cqueue_init_size(&m_session, size, BS_TRUE);
        cqueue_init_size(&m_status_queue, size, BS_TRUE);
    }
    ~UTPIOBase();
    
    inline UTPAddrQueue* addrQueue(const struct sockaddr_in* addr);
    inline _cqueue_t* sessionQueue() { return (_cqueue_t*)&m_session;}
    inline _cqueue_t* statusQueue() { return (_cqueue_t*)&m_status_queue; }
    
    // index为addr中队列的位置
    utp_pack_t* getPack(const struct sockaddr_in* addr, uint32_t addr_index);
    utp_elem_t* getElem(const struct sockaddr_in* addr, uint32_t addr_index);
    utp_head_t* getHead(const struct sockaddr_in* addr, uint32_t addr_index);
    
    inline void sendSignal(const struct sockaddr_in* addr, uint32_t signal, uint32_t para);
    inline void setSocket(int sock) { m_sock = sock; }
    inline void copyPack(utp_pack_t* src, const utp_pack_t* dst);
    //inline static uint32_t calCheckCode(const char* buf, uint32_t size);
protected:
    typedef map<uint64_t, UTPAddrQueue>  addr_map_t;
    addr_map_t                  m_addr_map;
    
    // 数据缓存区
    cqueue_t(utp_pack_t)        m_session;
    // 数据状态缓存区，reader用于检查是否可读，writer用于发送
    cqueue_t(utp_status_t)      m_status_queue;
    
    int                         m_sock;
};

class UTPReader:public UTPIOBase{
public:
    UTPReader(uint32_t size=UTP_DEF_SLICE_NUM):UTPIOBase(size){}
    
    int32_t setRead(const struct sockaddr_in* addr, const char* buf, uint32_t size);
    inline int32_t setPack(UTPAddrQueue* addr, const char* buf, uint32_t size);
    inline void checkHead(UTPAddrQueue* queue, const utp_elem_t* elem);
    void checkPack(bool new_pack);
    void sendReSignal(const struct sockaddr_in* addr, utp_elem_t* elem, uint32_t id);
};

class UTPWriter:public UTPIOBase{
public:
    UTPWriter(uint32_t size=UTP_DEF_SLICE_NUM):UTPIOBase(size){}
    // 返回包id
    uint32_t send(const struct sockaddr_in* addr, const void* buf, uint32_t size, uint8_t data_type = UTP_NULL, bool_t reply = BS_FALSE);
    state_t sendBlock(const struct sockaddr_in* addr, const void* buf, uint32_t size, uint8_t date_type = UTP_NULL);
    uint32_t sendPriority(const struct sockaddr_in* addr, const void* buf, uint32_t size, int data_type = UTP_NULL);
    
    //state_t sendSignal(const char* ip, int port, const void* buf, uint32_t size);
    state_t sendImmediately(const struct sockaddr_in* addr, const void* buf, uint32_t size, int data_type = UTP_NULL);
    
    // 发送的是utp_stattus_t，可能有问题，造成2次发送
    inline void sendPriority(const utp_status_t* status);
    
    void setSend(UTPAddrQueue* addr, const utp_pack_t* pack);
    inline void delDestination(const char* ip, int port);

    // 缓存区保留数量，用于填充signal及重发等优先级高的pack
    static uint32_t UTP_SESSION_RETAIN;
};

class UTPSocket: public AsyncSocket, public Thread{
public:
    static UTPSocket* instance() { return utp; }
    static UTPSocket* initialize(int port){
        if(utp == NULL){
            int sock = socket_udp(BS_TRUE);
            utp = new UTPSocket(sock, port);
        }
        return utp;
    }
    
    virtual void onRead();
    virtual void onWrite();
    virtual void onError(int error);
    virtual void onMessage(async::message_t* msg);
    
    void run();
    /**
     * 0:无就绪数据；>0：要读取的数据长度
     */
    int utp_select(int data_type = UTP_NULL);
    /**
     * -1:未读取；0:buf不足已读满；>0：实际读取大小
     */
    int utp_recv(char* buf, uint32_t size, int type=UTP_NULL);
    
    state_t sendThread();
    state_t send(const struct sockaddr_in* addr, const void* buf, uint32_t size, int data_type = UTP_NULL);
    state_t send(const char* ip, int port, const void* buf, uint32_t size, int data_type = UTP_NULL);
    //state_t sendSignal(const char* ip, int port, const void* buf, uint32_t size);
    state_t sendImmediately(const char* ip, int port, const void* buf, uint32_t size, int data_type = UTP_NULL){
        struct sockaddr_in      addr;
        
        bs_sock_addr(&addr, ip, port);
        return m_writer.sendImmediately(&addr, buf, size, data_type);
    }
    state_t sendImmediately(const sockaddr_in* addr, const void* buf, uint32_t size, int data_type = UTP_NULL){
        return m_writer.sendImmediately(addr, buf, size, data_type);
    }
    state_t sendPriority(const struct sockaddr_in* addr, const void* buf, uint32_t size, int data_type = UTP_NULL){
        if(m_writer.sendPriority(addr, buf, size, data_type)>0){
            return BS_SUCCESS;
        }
        return BS_INVALID;
    }
    
    // 调用sendPriority，sendPriority只能发单包，所以只能发单包
    state_t sendBlock(const struct sockaddr_in* addr, const void* buf, uint32_t size, int data_type = UTP_NULL){
        return m_writer.sendBlock(addr, buf, size, data_type);
    }
    
    void delDestination(const char* ip, int port);
    void flush(){
        cqueue_clear(m_reader.statusQueue());
        cqueue_clear(m_writer.statusQueue());
    }
    
    inline void setHandle(utp_data_type_t type, date_func_t handle) { handle_map[type] = handle; }
    inline UTPIOBase& getReader() { return m_reader;}
    inline UTPIOBase& getWriter() { return m_writer;}
    inline const char* getNatAddr() { return m_nat_addr;}
    inline int getSocket() { return m_sock; }
    inline int getPort() { return m_local_port; }
    //inline state_t getOtherSignal(utp_sig_other_t* sig) { return m_other_session.pop(sig);}
    // 发送线程周期
    static uint32_t     UTP_SEND_FREQ;
    // 重发次数
    static uint32_t     UTP_RESEND_NUM;
    // 检查超时时间
    static uint32_t     UTP_RESEND_TIMEOUT;
    
protected:
    UTPSocket(int sock, int port): AsyncSocket(sock, SOCK_DGRAM), m_reader(), m_writer(){
        m_sock = sock;
        bs_sock_bind(m_sock, port);
        m_local_port = port;
        
        m_reader.setSocket(m_sock);
        m_writer.setSocket(m_sock);
        FD_ZERO(&m_read_set);
        FD_SET(m_sock, &m_read_set);
        memset(handle_map, 0, sizeof(date_func_t)*UTP_TYPE_NUM);
    }
    
    inline void setSignal(const struct sockaddr_in* addr, const utp_signal_t* signal);
    //virtual int32_t setRead(struct sockaddr_in addr, const char* buf, uint32_t size);
    
    static UTPSocket*   utp;
    date_func_t         handle_map[UTP_TYPE_NUM];
    
    UTPReader           m_reader;
    UTPWriter           m_writer;
    
    int                 m_local_port;
    fd_set              m_read_set;
    bs_timer_t*         m_send_timer;
    
    char				m_nat_addr[32];
};
#endif
#endif /* defined(__utp__) */
