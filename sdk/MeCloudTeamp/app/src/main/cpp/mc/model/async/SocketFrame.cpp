/**
 * file :	SocketFrame.cpp
 * author :	bushaofeng
 * create :	2014-10-05 06:43
 * func :
 * history:
 */

#include "SocketFrame.h"

SocketFrame* SocketFrame::m_instance = NULL;

SocketFrame::SocketFrame(uint32_t timeout):AsyncFrame(timeout){
    memset(m_sock_map, 0, sizeof(m_sock_map));
    pthread_mutex_init(&m_lock, NULL);
}

state_t SocketFrame::append(AsyncSocket* socket){
    stop();
    pthread_mutex_lock(&m_lock);
    
    int sock = socket->getSocket();
    if (sock>=0xffff) {
        pthread_mutex_unlock(&m_lock);
        return BS_FULL;
    }
    m_sock_map[sock] = socket;
    FD_SET(sock, &m_read_set);
    // 在socket没有发送数据情况下，ios认为处于可写状态
    // FD_SET(sock, &m_write_set);
    FD_SET(sock, &m_error_set);
    if (m_max_sock <= sock) {
        m_max_sock = sock+1;
    }
    
    pthread_mutex_unlock(&m_lock);
    start();
    return BS_SUCCESS;
}

state_t SocketFrame::remove(AsyncSocket* socket){
    int sock = socket->getSocket();
    if (m_sock_map[sock] == NULL) {
        return BS_SUCCESS;
    }
    
    stop();
    pthread_mutex_lock(&m_lock);
    
    FD_CLR(sock, &m_read_set);
    FD_CLR(sock, &m_error_set);
    m_sock_map[sock] = NULL;
    close(socket->getSocket());
    
    pthread_mutex_unlock(&m_lock);
    start();
    return BS_SUCCESS;
}

state_t SocketFrame::isExist(AsyncSocket* socket){
    for (int i = 0;i<m_max_sock; i++) {
        if (m_sock_map[i] == NULL) {
            continue;
        }
        if (m_sock_map[i] == socket) {
            return BS_SUCCESS;
        }
    }
    return BS_INVALID;
}

void SocketFrame::loop(){
    pthread_mutex_lock(&m_lock);
    AsyncFrame::loop();
    if (m_select_rv>0){
        for (int sock = 0; sock < m_max_sock; sock++) {
            if (m_sock_map[sock] == NULL) {
                continue;
            }
            
            if (FD_ISSET(sock, &m_read_set)){
                m_sock_map[sock]->onRead();
            }
            else{
                FD_SET(sock, &m_read_set);
            }
            
            // if (FD_ISSET(iter->first, &m_write_set)){
            //     iter->second->onWrite();
            // }
            // else{
            //     FD_SET(iter->first, &m_write_set);
            // }
            if (FD_ISSET(sock, &m_error_set)) {
                m_sock_map[sock]->onError(errno);
            }
            else {
                FD_SET(sock, &m_error_set);
            }
        }
    }
    
    pthread_mutex_unlock(&m_lock);
}
