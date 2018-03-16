/**
 * file :	WebSocketClient.cpp
 * author :	bushaofeng
 * create :	2014-10-13 11:41
 * func : 
 * history:
 */

#include "Push.h"

Push*               Push::m_instance        = NULL;
WEBSocket*        Push::m_websocket       = NULL;
SocketFrame*        Push::m_frame           = NULL;
char                Push::m_user[push::USER_SIZE];
uint32_t            Push::m_identity = 1;
response_handle_t   Push::m_response_map[IDENTITY_MAP_SIZE] = {0};
void*               Push::m_parameter_map[IDENTITY_MAP_SIZE] = {0};
action_handle_t     Push::m_action_map[0xff] = {0};

Push* Push::instance(){
    if (m_instance == NULL) {
        m_instance = new Push;
        m_websocket = new WEBSocket(m_instance, 0);
        m_frame = SocketFrame::instance();
    }
    
    return m_instance;
}

state_t Push::login(const char* server,const char* sessionID){
    int st = BS_INVALID;
    state_t result = m_websocket->open(server,sessionID);
    if (result == BS_SUCCESS) {
        m_frame->append(m_websocket);
        st = BS_SUCCESS;
    }
    return st;
}

state_t Push::sendText(const char* to, const char* text, uint32_t size, response_handle_t handle, void* para){
    push::message_t     msg;
    push::message_t*    message;
    state_t             st;
    
    message = createMessage(&msg, push::ACTION_MESSAGE, to, text, size);
    st = m_websocket->sendText(message, requestSize(message));
    if (size > SOCKET_TCP_MTU) {
        delete[] message;
    }
    if (st == BS_SUCCESS) {
        m_response_map[msg.identifier&(IDENTITY_MAP_SIZE-1)] = handle;
        m_parameter_map[msg.identifier&(IDENTITY_MAP_SIZE-1)] = para;
    }
    
    return st;
}
state_t Push::sendBinary(const char* to, const void* buf, uint32_t size, response_handle_t handle, void* para){
    push::message_t     msg;
    push::message_t*    message;
    state_t             st;
    
    message = createMessage(&msg, push::ACTION_MESSAGE, to, buf, size);
    st = m_websocket->sendBinary(message, requestSize(message));
    if (size > SOCKET_TCP_MTU) {
        delete[] message;
    }
    if (st == BS_SUCCESS) {
        m_response_map[msg.identifier&(IDENTITY_MAP_SIZE-1)] = handle;
        m_parameter_map[msg.identifier&(IDENTITY_MAP_SIZE-1)] = para;
    }
    
    return st;
}

state_t Push::sendExpanded(const char* to, uint8_t msg_type, const void* buf, uint32_t size, response_handle_t handle, void* para){
    push::message_t     msg;
    push::message_t*    message;
    state_t             st;
    
    message = createMessage(&msg, msg_type, to, buf, size);
    st = m_websocket->sendBinary(message, requestSize(message));
    if (size > SOCKET_TCP_MTU) {
        delete[] message;
    }
    if (st == BS_SUCCESS) {
        m_response_map[msg.identifier&(IDENTITY_MAP_SIZE-1)] = handle;
        m_parameter_map[msg.identifier&(IDENTITY_MAP_SIZE-1)] = para;
    }
    
    return st;
}

push::message_t* Push::createMessage(push::message_t* msg, uint8_t msg_type, const char* to, const void* buf, uint32_t size){
    void*               buffer;
    
    if (size > SOCKET_TCP_MTU) {
        buffer = new char[size+offsetof(push::message_t, params)];
    }
    else{
        buffer = msg;
    }
    
    requestInit((push::message_t*)buffer, msg_type, m_identity++, m_user, to);
    requestBuffer((push::message_t*)buffer, buf, size);
    return (push::message_t*)buffer;
}
/**
 * 固定字段：action, from, to, params
 */
void Push::onOpen(WEBSocket* socket){
    push::message_t     msg;
    requestInit(&msg, push::ACTION_LOGIN, m_user, NULL);
    m_websocket->sendText(&msg, requestSize(&msg));
}

void Push::onOpen(WEBSocket* socket,const char* sessionId){}

void Push::onText(WEBSocket* socket, char* buf, uint64_t size){
    push::message_t*    msg = (push::message_t*)buf;
    uint32_t            pos = getMapPos(msg->identifier);
    
    msg->params[msg->size] = 0;
    //push::print_message(msg);
    switch (msg->action) {
        case push::ACTION_LOGIN:
            if (msg->result != push::RESULT_OK) {
                err_log("PUSH LOGIN ERROR");
                return;
            }
            notice_log("%s login success!!!",msg->from);
            break;
        case push::ACTION_LOGOUT:
            break;
        case push::ACTION_MESSAGE:
            if (msg->result==push::RESULT_NULL) {
                // android中为jni调用java代码发送通知
                if (m_action_map[msg->action]) {
                    m_action_map[msg->action](msg);
                }
            }
            else if (m_response_map[pos]){
                // 执行异步函数，执行完成后删除
                m_response_map[pos](msg->result, m_parameter_map[pos]);
                m_response_map[pos] = NULL;
            }
            break;
        default:
            if (msg->result==push::RESULT_NULL && m_action_map[msg->action]) {
                m_action_map[msg->action](msg);
            }
            else if (msg->result!=push::RESULT_NULL && m_response_map[pos]){
                // 执行异步函数
                m_response_map[pos](msg->result, m_parameter_map[pos]);
            }
            break;
    }
}

void Push::onBinary(WEBSocket* socket, char* buf, uint64_t size){
    push::message_t*    msg = (push::message_t*)buf;
    uint32_t            pos = getMapPos(msg->identifier);
    
    push::print_message(msg);
    switch (msg->action) {
        case push::ACTION_MESSAGE:
            if (msg->result==push::RESULT_NULL) {
                if (m_action_map[msg->action]) {
                    m_action_map[msg->action](msg);
                }
                debug_log("recv binary msg[%d] from[%s]", msg->size, msg->from);
            }
            else if (m_response_map[pos]){
                // 执行异步函数
                m_response_map[pos](msg->result, m_parameter_map[pos]);
            }
            break;
        default:
            if (msg->result==push::RESULT_NULL && m_action_map[msg->action]) {
                m_action_map[msg->action](msg);
            }
            else if (msg->result!=push::RESULT_NULL && m_response_map[pos]){
                // 执行异步函数
                m_response_map[pos](msg->result, m_parameter_map[pos]);
            }
            break;
    }
}

void Push::onClose(WEBSocket* socket){
    close(socket->getSocket());
    m_frame->stop();
    m_frame->remove(socket);
}

void Push::onError(WEBSocket* error){
    m_frame->stop();
    m_frame->remove(error);
}

state_t Push::onExist(const char *sessionId)
{
    return BS_SUCCESS;
}
