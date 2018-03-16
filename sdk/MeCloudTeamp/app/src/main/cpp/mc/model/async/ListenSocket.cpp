/**
 * file :	ListenSocket.cpp
 * author :	Rex
 * create :	2017-02-25 14:52
 * func : 
 * history:
 */

#include "ListenSocket.h"

void TCPSocket::onRead(){
    int                 sock;
    socklen_t           addrlen;
    struct sockaddr_in  addr;
    
    addrlen = sizeof(struct sockaddr_in);
    assert((sock = accept(m_sock, (struct sockaddr*)&addr, &addrlen))>0);
    if (sock>0) {
        debug_log("accept sock: %d", sock);
        sock_msg_t* msg = bs_new(sock_msg);
        if (msg!=NULL){
            memcpy(msg->buf, &sock, sizeof(sock));
            msg->size = sizeof(sock);
            msg->sock = m_sock;
            msg->arg = this;
            async_run(sock_on_message, msg);
        }
    }
}

void UDPSocket::onRead(){
    sock_msg_t* msg = bs_new(sock_msg);
    int len = bs_sock_recvfrom(m_sock, &msg->addr, msg->buf, sizeof(msg->temp));
    if (len<0) {
        bs_delete(msg);
        return;
    }
    
    msg->sock = m_sock;
    msg->size = len;
    msg->arg = this;
    async_run(sock_on_message, msg);
}
