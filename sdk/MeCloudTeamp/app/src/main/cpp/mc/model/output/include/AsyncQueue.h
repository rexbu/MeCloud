/**
 * file :	AsyncQueue.h
 * author :	bushaofeng
 * create :	2015-03-22 00:09
 * func :   异步阻塞式Queue，用select控制queue的可读
 * history:
 */

#ifndef	__ASYNCQUEUE_H_
#define	__ASYNCQUEUE_H_

#include "bs.h"
#include "basic.h"
#include "AsyncFrame.h"

typedef void (* async_handle_t)(void* buffer);

class AsyncQueue:public AsyncFrame{
public:
    AsyncQueue(){}
    AsyncQueue(async_handle_t handle, uint32_t esize, uint32_t size = CQUEUE_DEF_SIZE){
        init(handle, esize, size);
    }
    // TODO 外部传入queue，用于多线程并发处理一个Queue
    AsyncQueue(_cqueue_t* queue, async_handle_t handle, uint32_t esize, uint32_t size = CQUEUE_DEF_SIZE){
        init(queue, handle, esize, size);
    }
    
    state_t init(async_handle_t handle, uint32_t esize, uint32_t size = CQUEUE_DEF_SIZE);
    state_t init(_cqueue_t* queue, async_handle_t handle, uint32_t esize, uint32_t size = CQUEUE_DEF_SIZE);
    
    void* push(void* buffer){
        void* ptr = cqueue_push(m_queue, buffer);
        if (ptr!=NULL) {
            interrupt();
        }

        return ptr;
    }
    
    _cqueue_t* getQueue(){
        return m_queue;
    }
    
protected:
    void interruptHandle();
    
protected:
    _cqueue_t*      m_queue;
    _cqueue_t       m_queue_entity;
    async_handle_t  m_handle;
    char            m_buffer[SOCKET_TCP_MTU];
};

#endif
