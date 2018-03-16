/**
 * file :	AsyncSocket.cpp
 * author :	bushaofeng
 * create :	2014-10-16 12:35
 * func : 
 * history:
 */

#include "AsyncSocket.h"
#include "ThreadPool.h"

void* sock_msg_init(void* p){
    sock_msg_t* msg = (sock_msg_t*)p;
    
    msg->buf = msg->temp;
    return msg;
}

void sock_msg_destroy(void* p){
    sock_msg_t* msg = (sock_msg_t*)p;
    // 如果buf是额外申请的空间
    if (msg->buf != msg->temp && msg->buf!=NULL) {
        free(msg->buf);
    }
}

// msg异步执行
void* sock_on_message(void* arg){
    sock_msg_t* msg = (sock_msg_t*)arg;
    AsyncSocket* sock = (AsyncSocket*)msg->arg;
    sock->onMessage(msg);
    // 如果msg的buf是另外申请的
    bs_delete(msg);
    return NULL;
}

AsyncSocket::AsyncSocket(int sock, int sock_type):
m_sock(sock),m_type(sock_type){}

AsyncSocket::AsyncSocket(){
    m_sock = 0;
    m_type = SOCK_STREAM;
}

void AsyncSocket::onRead(){
    sock_msg_t* msg = bs_new(sock_msg);
    if (msg!=NULL){
        msg->size = (int)read(m_sock, msg->temp, sizeof(msg->temp));
        debug_log("socket[%d] recv [%d]", m_sock, msg->size);
        if (msg->size <= 0) {
            err_log("socket[%d] recv[%d] error", m_sock, msg->size);
            onError(errno);
            return;
        }
        msg->buf = msg->temp;
        msg->sock = m_sock;
        msg->arg = this;
        async_run(sock_on_message, msg);
    }
}
