/**
 * file :	ListenSocket.h
 * author :	bushaofeng
 * create :	2014-10-09 18:36
 * func : 
 * history:
 */

#ifndef	__LISTENSOCKET_H_
#define	__LISTENSOCKET_H_

#include "bs.h"
#include "AsyncSocket.h"
#include "SocketFrame.h"

class ListenSocket: public AsyncSocket{
public:
    ListenSocket(int port):m_port(port){
        m_sock = socket_tcp(BS_TRUE);
        bs_sock_bind(m_sock, m_port);
        listen(m_sock, BACKLOG_SIZE);
    }
    
    void onRead(){
        int                 sock;
        socklen_t           addrlen;
        struct sockaddr_in  addr;
        
        addrlen = sizeof(struct sockaddr_in);
        assert((sock = accept(m_sock, (struct sockaddr*)&addr, &addrlen))>0);
        if (sock>0) {
            debug_log("accept sock: %d", sock);
            async::message_t        msg;
            
            msg.buf = (char*)pool_malloc(&m_read_pool);
            if (msg.buf!=NULL){
                memcpy(msg.buf, &sock, sizeof(sock));
                msg.size = sizeof(sock);
                msg.sock = m_sock;
                msg.para = this;
                // message写入异步消息队列
                m_message_queue.push(&msg);
            }
        }
    }
    void onWrite(){}
    void onError(int error){}
    void onMessage(async::message_t* msg){
        int sock = *((int*)msg->buf);
        AsyncSocket* socket = createSocket(sock);
        SocketFrame::instance()->append(socket);
        pool_free(&m_read_pool, msg->buf);
    }
    
    // 纯虚函数，需要实现的生成AsyncSocket类型
    virtual AsyncSocket* createSocket(int sock) = 0;
    const static int BACKLOG_SIZE = 64;
    
protected:
    int         m_port;
};

#endif
