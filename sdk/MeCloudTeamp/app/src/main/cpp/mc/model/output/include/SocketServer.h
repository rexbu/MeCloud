/**
 * file :	SocketServer.h
 * author :	bushaofeng
 * create :	2015-03-22 11:31
 * func :
 * history:
 */

#ifndef __SOCKETSERVER_H_
#define __SOCKETSERVER_H_

#include "AsyncSocket.h"
#include "SocketFrame.h"
#include "ListenSocket.h"

class SocketServer:public SocketFrame{
public:
    SocketServer* initialize(ListenSocket* socket, uint32_t timeout=0){
        if (m_instance==NULL){
            m_instance = new SocketServer(socket, timeout);
        }
        return m_instance;
    }
    SocketServer* instance(){
        assert(m_instance != NULL);
        return m_instance;
    }
    
    SocketServer(ListenSocket* socket, uint32_t timeout=0):SocketFrame(timeout){
        append(socket);
    }
protected:
    static SocketServer*   m_instance;
    
    int     m_port;
};

#endif
