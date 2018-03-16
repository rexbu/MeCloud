/**
 * file :	AsyncSocket.h
 * author :	bushaofeng
 * create :	2014-10-04 22:39
 * func :   一次发包不能超过mtu大小
 * history:
 */

#ifndef	__ASYNCSOCKET_H_
#define	__ASYNCSOCKET_H_

#include "bs.h"
#include "basic.h"
#include "Thread.h"
#include "AsyncQueue.h"
#ifdef __ANDROID__
#include <jni.h>
#endif

namespace async {
    typedef struct message_t{
        int                 sock;
        uint32_t            size;
        char*               buf;
        struct sockaddr_in  addr;
        // 额外参数，目前用法指向AsyncSocket，用于AsyncQueue的回调
        void*               para;
    } message_t;
    
    static const uint32_t       SOCKET_POOL_DEF_SIZE = 4096;
    // 监听socket的等待连接队列
    static const uint32_t       SOCKET_LISTEN_QUEUE_SIZE = 1024;
}

// AsyncQueue处理函数
void async_queue_handle(void* buffer);

class AsyncSocket{
public:
    AsyncSocket(int sock, int sock_type = SOCK_STREAM);
    AsyncSocket();
    virtual ~AsyncSocket(){}
    
    int getSocket() {
        return m_sock;
    }
    int getSocketType() {
        return m_type;
    }
    
    static async::message_t* obtainMessage();
    
    virtual void onRead();
    virtual void onWrite() = 0;
    virtual void onError(int error) = 0;
    virtual void onMessage(async::message_t* msg) = 0;
    
    uint32_t getMtu(){
        return m_mtu;
    }
    
    // 初始化全局，包括数据缓存和消息队列
    static state_t initialize(uint32_t pool_size, uint32_t mtu);
    
    // 读取数据缓存及消息队列
    static _pool_t      m_read_pool;
    static AsyncQueue   m_message_queue;

protected:
    static bool         m_init_flag;
    // 相同socket mtu必须一样
    static uint32_t     m_mtu;
    
    int                 m_sock;
    
    int                 m_newscok;
    int                 m_type;
};

#endif
