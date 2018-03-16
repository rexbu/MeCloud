/**
 * file :	AsyncFrame.cpp
 * author :	bushaofeng
 * create :	2015-03-23 16:59
 * func :
 * history:
 */

#include "AsyncFrame.h"

AsyncFrame::AsyncFrame(uint32_t timeout, bool ispipe){
    m_ispipe = ispipe;
    m_frame_state = FRAME_STATE_IDLE;
    if (ispipe) {
        // m_fd用于控制select的立刻返回
        int         optval = 0;
        uint32_t    len = sizeof(unsigned int);
        assert(pipe(m_pipe)==0);
        //禁用NAGLE算法
        setsockopt(m_pipe[0], IPPROTO_TCP, TCP_NODELAY, &optval, len);
        setsockopt(m_pipe[1], IPPROTO_TCP, TCP_NODELAY, &optval, len);
        
        m_max_sock = m_pipe[0]>m_pipe[1] ? (m_pipe[0]+1):(m_pipe[1]+1);
        FD_ZERO(&m_read_set);
        FD_ZERO(&m_write_set);
        FD_ZERO(&m_error_set);
        FD_SET(m_pipe[0], &m_read_set);
        
        fprintf(stdout, "pipe[%d/%d] max_sock[%d]\n", m_pipe[0], m_pipe[1], m_max_sock);
    }
    
    if (timeout==0) {
        m_timeout = NULL;
    }
    else{
        m_timeout = &m_time_entity;
        BS_SET_TIMEVAL(m_timeout, timeout);
    }
}

void AsyncFrame::stop(){
    if (m_running) {
        LoopThread::stop();
        interrupt();
    }
}
void AsyncFrame::interrupt(){
    write(m_pipe[1], "a", 1);
}

void AsyncFrame::loop(){
    char            buffer[8] = {0};
    
    m_frame_state = FRAME_STATE_LISTEN;
    m_select_rv = select(m_max_sock, &m_read_set, &m_write_set, &m_error_set, m_timeout);
    m_frame_state = FRAME_STATE_HANDLE;
    if (m_select_rv>0) {
        if (!m_ispipe) {
            return;
        }
        
        if (FD_ISSET(m_pipe[0], &m_read_set)) {
            read(m_pipe[0], buffer, sizeof(buffer));
            interruptHandle();
        }
        else{
            FD_SET(m_pipe[0], &m_read_set);
        }
    }
}
