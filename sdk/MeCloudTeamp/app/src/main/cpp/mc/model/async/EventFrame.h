/**
 * file :	EventFrame.h
 * author :	bushaofeng
 * create :	2014-10-05 06:43
 * func :
 * history:
 */

#ifndef __EVENTFRAME_H_
#define __EVENTFRAME_H_

#include "bs.h"
#include "basic.h"
#include "EventAsyncSocket.h"

class EventFrame{
public:
    static EventFrame* instance(){
        return m_instance;
    }
    
    /*
    EventFrame(uint32_t size){
        m_read_session = new CLockQueue<async::message_t>(size);
        m_write_session = new CLockQueue<async::message_t>(size);
    }
    */
    
    // 打开一个tcp监听
    state_t open(int port);
    // 无监听，直接往循环中添加event
    state_t open(){
        m_base = event_base_new();
        if (m_base == NULL) {
            return BS_INVALID;
        }
        
        return BS_SUCCESS;
    }
    
    struct event_base*  getEventBase() {
        return m_base;
    }
    
    void start(){
        event_base_dispatch(m_base);
    }
    
    void append(EventAsyncSocket* socket);
    // 创建新的EventAsyncSocket，子类中实现以创建不同协议的socket
    virtual EventAsyncSocket* createSocket(int sock) = 0;
    virtual void deleteSocket(EventAsyncSocket* socket) = 0;
    void isExist(EventAsyncSocket* socket);
    
protected:
    static EventFrame*                  m_instance;
    /*
    static CLockQueue<async::message_t>*m_read_session;
    static CLockQueue<async::message_t>*m_write_session;
    */
    
protected:
    int                 m_sock;
    int                 m_port;
    struct event_base*  m_base;
    struct event        m_listen_event;
};
#endif
