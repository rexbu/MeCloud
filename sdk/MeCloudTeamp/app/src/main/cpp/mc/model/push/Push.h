/**
 * file :	Push.h
 * author :	bushaofeng
 * create :	2014-10-13 11:41
 * func :   客户端WebSocket接口
 * history:
 */

#ifndef	__PUSH_H_
#define	__PUSH_H_

#include "WebSocket.h"
#include "SocketFrame.h"
//#include "hiredis.h"

namespace push{
    // 最多256个action，0-9预留，10以后开放注册，自定义
    enum action_t{
        ACTION_LOGIN,
        ACTION_LOGOUT,
        ACTION_STATUS,
        ACTION_MESSAGE,
        ACTION_NUM
    };

    enum result_t{
        RESULT_NULL,
        RESULT_OK,
        RESULT_INVALID,
        RESULT_USERERR,
        RESULT_PASSERR,
        RESULT_OUTLINE
    };
    
    static const int SESSION_SIZE   = 1024;
    static const int USER_SIZE      = 40;
    
    /**
     * action: 动作；result: 用于服务端返回结果判断; identifier:区分不同请求，用于异步响应; size:params的长度; from:发自; to:发给，发给server端此字段为空
     */
    typedef struct message_t{
        uint8_t         action;
        uint8_t         result;
        uint32_t        identifier;
        uint32_t        size;
        char            from[USER_SIZE];
        char            to[USER_SIZE];
        //char            *params;
        //char            params[URL_SIZE];
        char            params[SOCKET_TCP_MTU-92];
    }message_t;
    
    inline void requestInit(message_t* msg, int action, const char* from, const char* to){
        memset(msg, 0, sizeof(message_t));
        msg->action = 0;
        bs_strcpy(msg->from, USER_SIZE, from);
        bs_strcpy(msg->to, USER_SIZE, to);
    }
    inline void requestInit(message_t* msg, int action, uint32_t id, const char* from, const char* to){
        requestInit(msg, action, from, to);
        msg->identifier = id;
    }
    inline void requestBuffer(message_t* msg, const void* buf, uint32_t size){
        memcpy(msg->params, buf, size);
        msg->size = size;
    }
    inline uint32_t requestSize(const message_t* msg){
        return offsetof(message_t, params)+msg->size;
    }
    inline void responseInit(message_t* msg, int action, int result){
        msg->action = action;
        msg->result = result;
        msg->identifier = 0;
    }
    inline void responseInit(message_t* msg, uint32_t id, int action, int result){
        msg->action = action;
        msg->result = result;
        msg->identifier = id;
    }
    inline uint32_t responseSize(message_t* msg){
        return offsetof(message_t, size);
    }
    
    static const char* action_string[] = {
        "Login",
        "Logout",
        "Status",
        "Message",
        "P2P"
    };
    static const char* result_string[] = {
        "NULL",
        "OK",
        "INVALID",
        "USERERR",
        "PASSERR",
        "OUTLINE"
    };
    inline void print_message(const push::message_t* msg){
        if (msg->result == RESULT_NULL) {
            if (msg->action <= ACTION_NUM) {
                //char show[131];
                //strncpy(show, msg->params, 130);
                //char* mm = msg->params;
                //show[130] = '\0';
                //debug_log("action[%s] result[%s] id[%u] from[%s] to[%s] params_size[%u] params[%s]",
                  //        action_string[msg->action], result_string[msg->result], msg->identifier, msg->from, msg->to, msg->size, msg->params);
                debug_log("action[%s] result[%s] id[%u] from[%s] to[%s] params_size[%u]",
                          action_string[msg->action], result_string[msg->result], msg->identifier, msg->from, msg->to, msg->size);
            }
            else {
                debug_log("action[expanded] result[%s] id[%u] from[%s] to[%s] params_size[%u] params[%s]",
                          result_string[msg->result], msg->identifier, msg->from, msg->to, msg->size, msg->params);
            }
        }
        else{
            if (msg->action <= ACTION_NUM) {
                debug_log("action[%s] result[%s] id[%u]",
                          action_string[msg->action], result_string[msg->result], msg->identifier);
            }
            else {
                debug_log("action[expanded] result[%s] id[%u]",
                          result_string[msg->result], msg->identifier);
            }
        }
    }
};

// 发送成功函数
typedef void (* response_handle_t)(state_t st, void* para);
typedef void (* action_handle_t)(push::message_t* msg);

class Push: public WebSocketListener{
public:
    static Push* initialize(const char* server, const char* user,const char* sessionId){
        if (m_instance == NULL) {
            m_instance = instance();
            setUser(user);
            login(server,sessionId);
        }
        else{
            logout();
            login(server, sessionId);
        }
        
        return m_instance;
    }
    
    static Push* instance();
    static void setUser(const char* user){
        bs_strcpy(m_user, push::USER_SIZE, user);
    }
    static const char* getUser(){
        return m_user;
    }
    static WEBSocket* getWebsocket(){
        return m_websocket;
    }
    
    //static void setsessionID(const char* sessionID){
    //    bs_strcpy(m_session, push::SESSION_SIZE, sessionID);
    //}
    static state_t login(const char* server,const char* sessionId);
    static void logout(){
        close(m_websocket->getSocket());
        m_frame->stop();
        m_frame->remove(m_websocket);
    }
    
    //校验sessionID函数
    //static bool onCheckSessionID(const char* sessionID);
    
    static state_t sendText(const char* to, const char* text, uint32_t size, response_handle_t handle=NULL, void* para=NULL);
    static state_t sendBinary(const char* to, const void* text, uint32_t size, response_handle_t handle=NULL, void* para=NULL);
    static state_t sendExpanded(const char* to, uint8_t msg_type, const void* buf, uint32_t size, response_handle_t handle=NULL, void* para=NULL);
    static push::message_t* createMessage(push::message_t* msg, uint8_t msg_type, const char* to, const void* buf, uint32_t size);
    
    void onOpen(WEBSocket* socket);
    void onOpen(WEBSocket* socket,const char* sessionId);
    
    void onText(WEBSocket* socket, char* buf, uint64_t size);
    //void onText(WEBSocket* socket, push::message_t* msg, uint64_t size);
    void onBinary(WEBSocket* socket, char* buf, uint64_t size);
    void onClose(WEBSocket* socket);
    void onError(WEBSocket* socket);
    state_t onExist(const char* sessionId);
    void setPingTime(const char* userId, char* pingtime){}
    void onSendOK(const char* userId){}
    void onResend(const char* userId){}
    // IDENTITY_MAP_SIZE必须是2的次幂，这样在计算m_response_map位置时直接用&运算法
    const static uint32_t       IDENTITY_MAP_SIZE = 1024;
    uint32_t getMapPos(uint32_t id){
        return id&(IDENTITY_MAP_SIZE-1);
    }
    void setHandle(uint8_t action, action_handle_t handle){
        m_action_map[action] = handle;
    }
    
protected:
    static Push*                m_instance;
    static WEBSocket*           m_websocket;
    static SocketFrame*         m_frame;
    static char                 m_user[push::USER_SIZE];
    //static char                 m_user[push::USER_SIZE];
    //static char                 m_session[push::SESSION_SIZE];
    // m_identity累积
    static uint32_t             m_identity;
    // 异步结果处理函数映射
    static response_handle_t    m_response_map[IDENTITY_MAP_SIZE];
    // 处理函数参数映射
    static void*                m_parameter_map[IDENTITY_MAP_SIZE];
    static action_handle_t      m_action_map[0xff];
};

#endif
