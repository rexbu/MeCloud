/**
 * file :	EventAsyncSocket.h
 * author :	bushaofeng
 * create :	2014-10-05 22:48
 * func : 
 * history:
 */

#ifndef	__EVENTASYNCSOCKET_H_
#define	__EVENTASYNCSOCKET_H_

#include <event.h>
#include "AsyncSocket.h"

class EventAsyncSocket:virtual public AsyncSocket{
public:
    EventAsyncSocket(int sock, int sock_type):AsyncSocket(sock, sock_type){}
    virtual ~EventAsyncSocket(){}
    
    struct event* getReadEvent() { return &m_read_event; }
    struct event* getWriteEvent() { return &m_write_event; }
    
protected:
    struct event    m_read_event;
    struct event    m_write_event;
};

#endif
