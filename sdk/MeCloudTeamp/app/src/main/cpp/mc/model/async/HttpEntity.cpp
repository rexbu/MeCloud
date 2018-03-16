/**
 * file :	Http.cpp
 * author :	Rex
 * create :	2017-03-16 14:04
 * func : 
 * history:
 */

#include "HttpEntity.h"

static void* http_error(void* arg);
static void* http_perform(void* arg);

#pragma --mark "基于bs_url和SocketFrame的HttpEntity"
using namespace model;

HttpEntity::HttpEntity(HttpCallback* callback, ThreadPool* thread, SocketFrame* frame){
    m_callback = callback;
    if (thread!=NULL) {
        m_http_thread = thread;
    }
    else{
        m_http_thread = ThreadPool::shareInstance();
    }
    
    if (frame!=NULL) {
        m_sock_frame = frame;
    }
    else{
        m_sock_frame = SocketFrame::instance();
    }
    
    m_sock = -1;
    memset(m_current_domain, 0, sizeof(m_current_domain));
    m_current_port = 0;
}

HttpEntity::~HttpEntity(){
    if (m_sock>0) {
        m_sock_frame->remove(this);
        ::close(m_sock);
    }
}

state_t HttpEntity::get(const char* url){
    return BS_SUCCESS;
}

state_t HttpEntity::post(const char* url, const char* body, uint32_t length){
    return http(url, "POST", body, length);
}

state_t HttpEntity::put(const char* url, const char* body, uint32_t length){
    return BS_SUCCESS;
}

state_t HttpEntity::del(const char* url){
    return BS_SUCCESS;
}

state_t HttpEntity::http(const char* url, const char* method, const char* body, uint32_t body_size){
    http_t* m_http = bs_new(http);
    url_init(&m_http->url);
    url_parse(&m_http->url, url);
    
    char buffer[URL_SIZE*2];
    snprintf(buffer, URL_SIZE*2, "%s %s HTTP/1.1\r\nAccept: */*\r\nHost: %s\r\nContent-Length: %d\r\nConnection: Keep-Alive\r\n", method, m_http->url.path.mem, m_http->url.host.mem, body_size);
    data_set(&m_http->req, buffer, (uint32_t)strlen(buffer));
    /* 加头
    for (i=0; i<bs_kv_size(header); i++) {
        snprintf(buffer, URL_SIZE*2, "%s: %s\r\n", bs_kv_getname(header, i), bs_kv_getstr_idx(header, i));
        data_append(&http->req, buffer, (uint32_t)strlen(buffer));
    }
    */
    
    data_append(&m_http->req, "\r\n", 2);
    if (body!=NULL && body_size!=0) {
        m_http->body = m_http->req.mem + m_http->req.len;
        data_append(&m_http->req, body, body_size);
        m_http->body_size = body_size;
    }
    
    void** arg = (void**)malloc(sizeof(void*)*2);
    arg[0] = this;
    arg[1] = m_http;
    
    m_http_thread->add(http_perform, arg);
    return BS_SUCCESS;
}
void HttpEntity::close(){
    m_current_port = 0;
    memset(m_current_domain, 0, sizeof(m_current_domain));
    m_sock_frame->remove(this);
    ::close(m_sock);
    m_sock = -1;
}

state_t HttpEntity::perform(http_t* http){
    // 判断是否是同一个连接
    if(http->url.port != m_current_port || strcmp(http->url.domain.mem, m_current_domain)!=0){
        if (m_sock>0) {
            close();
        }
        
        m_sock = socket_tcp(BS_FALSE);
        int nRecvBuf=64*1024;
        setsockopt(m_sock, SOL_SOCKET,SO_RCVBUF,(char*)&nRecvBuf,sizeof(int));
        int nSendBuf=1024*1024;
        setsockopt(m_sock, SOL_SOCKET,SO_SNDBUF,(char*)&nSendBuf,sizeof(int));
        
        int st = bs_sock_connect(m_sock, http->url.domain.mem, http->url.port);
        // EINPROGRESS表示连接还未完成
        if (st != BS_SUCCESS && st != EINPROGRESS) {
            ::close(m_sock);
            m_sock = -1;
            return BS_CONNERR;
        }
        
        memcpy(m_current_domain, http->url.domain.mem, http->url.domain.len);
        m_current_domain[http->url.domain.len] = '\0';
        m_current_port = http->url.port;
        // 设置为非阻塞模式，并加入异步
        socket_unblock(m_sock);
        m_sock_frame->append(this);
    }
    
    int len = (int)send(m_sock, http->req.mem, http->req.len, 0);
    if (len==0) {
        // 返回0时候表示连接被关闭
        return BS_CONNERR;
    }
    else if (len<0) {
        onError(errno);
    }
//    char buffer[512];
//    len = (int)recv(m_sock, buffer, 512, 0);
    return len;
}

void HttpEntity::addHttpHeader(const char* key, const char* value){
}

void HttpEntity::onRead(){
    sock_msg_t* msg = bs_new(sock_msg);
    if (msg==NULL){
        err_log("sock message malloc error!");
    }
    
    // 目前只处理h264
    msg->size = (int)recv(m_sock, msg->buf, 150, 0);
    if (msg->size<=0) {
        onError(errno);
        return;
    }
    
    msg->sock = m_sock;
    msg->arg = this;
    async_run(sock_on_message, msg);
}

void HttpEntity::onWrite(){
    debug_log("write success");
}

void HttpEntity::onError(int error){
    if ((error == EINTR || error == EWOULDBLOCK || error == EAGAIN)) {
        err_log("暂时扔掉");
    }
    else{
        err_log("http出错: %d, sock: %d", error, m_sock);
        async_run(http_error, this);
    }
}

void HttpEntity::onMessage(sock_msg_t* msg){
    debug_log("recv: %d", msg->size);
    if (m_callback!=NULL) {
        m_callback->done(HTTP_OK, BS_SUCCESS, msg->buf);
    }
}

#pragma --mark "http异步执行线程"
static void* http_error(void* arg){
    HttpEntity* entity = (HttpEntity*)arg;
    
    if (entity->m_callback!=NULL) {
        entity->m_callback->done(-1, BS_INVALID, NULL);
    }
    
    return NULL;
}
static void* http_perform(void* arg){
    void** argv = (void**)arg;
    
    HttpEntity* entity = (HttpEntity*)argv[0];
    http_t* http = (http_t*)argv[1];
    
    int len = entity->perform(http);
    if (len<0) {
        entity->onError(errno);
    }
    
    bs_delete(http);
    free(argv);
    
    return NULL;
}
