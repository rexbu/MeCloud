/**
 * file :	McTask.cpp
 * author :	Rex
 * create :	2016-11-03 17:00
 * func : 
 * history:
 */

#include "McTask.h"

using namespace mc;

#pragma --mark "异步任务队列"

AsyncTaskQueue::AsyncTaskQueue(uint32_t timeout){
    m_processing = false;
    pthread_mutex_init(&m_lock, NULL);
    
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
    
    debug_log("async pipe[%d/%d] max_sock[%d]\n", m_pipe[0], m_pipe[1], m_max_sock);
    if (timeout==0) {
        m_timeout = NULL;
    }
    else{
        m_timeout = &m_time_entity;
        BS_SET_TIMEVAL(m_timeout, timeout);
    }
}

void AsyncTaskQueue::stop(){
    if (m_running) {
        LoopThread::stop();
        write(m_pipe[1], "a", 1);
    }
}

void AsyncTaskQueue::add(void* (run)(void*), void* para){
    write(m_pipe[1], "a", 1);
    
    async_task_t task;
    task.run = run;
    task.argv = para;
    pthread_mutex_lock(&m_lock);
    m_tasks.push_back(task);
    pthread_mutex_unlock(&m_lock);
}

void AsyncTaskQueue::loop(){
    char            buffer[8] = {0};
    
    m_select_rv = select(m_max_sock, &m_read_set, &m_write_set, &m_error_set, m_timeout);
    if (m_select_rv>0) {
        m_processing = true;
        if (FD_ISSET(m_pipe[0], &m_read_set)) {
            read(m_pipe[0], buffer, sizeof(buffer));
            
            pthread_mutex_lock(&m_lock);
            vector<async_task_t>::iterator iter=m_tasks.begin();
            if (iter!=m_tasks.end()) {
                iter->run(iter->argv);
                
                m_tasks.erase(iter);
            }
            pthread_mutex_unlock(&m_lock);
//            for(vector<async_task_t>::iterator iter=m_tasks.begin(); iter!=m_tasks.end(); iter++){
//                info_log("task!!!");
//                iter->run(iter->argv);
//                
//                pthread_mutex_lock(&m_lock);
//                m_tasks.erase(iter);
//                pthread_mutex_unlock(&m_lock);
//            }
//            info_log("task!!!");
        }
        else{
            FD_SET(m_pipe[0], &m_read_set);
        }
        m_processing = false;
    }
}

#pragma --mark "异步框架"
AsyncFrame* AsyncFrame::m_instance = NULL;

void AsyncFrame::initialize(int queueNum){
    m_instance = new AsyncFrame(queueNum);
}

AsyncFrame* AsyncFrame::shareInstance(){
    if (m_instance==NULL) {
        initialize(2);
    }
    
    return m_instance;
}

void AsyncFrame::destroyInstance(){
    if (m_instance!=NULL) {
        delete m_instance;
    }
}

AsyncFrame::AsyncFrame(int queueNum){
    for (int i=0; i<queueNum; i++) {
        AsyncTaskQueue* queue = new AsyncTaskQueue;
        m_task_queue.push_back(queue);
        queue->start();
    }
}

AsyncFrame::~AsyncFrame(){
    for (int i=0; i<m_task_queue.size(); i++) {
        delete m_task_queue[i];
    }
    
    m_task_queue.clear();
}

// 寻找队列中任务最少的加入
void AsyncFrame::addTask(mc_async_task_f run, void* para){
    int min = 0;
    for (int i=1; i<m_task_queue.size(); i++) {
        if(m_task_queue[i]->taskNum()<m_task_queue[min]->taskNum()){
            min = i;
        }
    }
    
    m_task_queue[min]->add(run, para);
}
