/**
 * file :	WebSocket.h
 * author :	bushaofeng
 * create :	2014-10-05 23:26
 * func : 
 * history:
 */

#ifndef	__WEBSOCKET_H_
#define	__WEBSOCKET_H_

#include "bs.h"
#include "AsyncSocket.h"
#include <sys/time.h>
#include <time.h>
//#include "hiredis.h"
#define DELAY_TIME 20

typedef struct ws_protocal_t{
    uint8_t         fin:1;
    uint8_t         rsv:3;
    uint8_t         opcode:4;
    uint8_t         mask:1;
    uint8_t         payload_len:7;
    uint16_t        payload_len_16;
    uint64_t        payload_len_64;
    uint32_t        masking_key;
    const char*     payload;
    const char*     buffer;
}ws_protocal_t;

class WEBSocket;
class WebSocketListener{
public:
    virtual void onOpen(WEBSocket* socket) = 0;
    virtual void onOpen(WEBSocket* socket,const char* sessionId) = 0;
    virtual void onText(WEBSocket* socket, char* buf, uint64_t size) = 0;
   // virtual void onText(WEBSocket* socket, push::message_t* msg, uint64_t size)=0;
    virtual void onBinary(WEBSocket* socket, char* buf, uint64_t size) = 0;
    virtual void onClose(WEBSocket* socket) = 0;
    virtual void onError(WEBSocket* socket) = 0;
    virtual state_t onExist(const char* sessionId) = 0;
    virtual void setPingTime(const char* userId, char* pingtime) = 0;
    virtual void onResend(const char* userId) = 0;
    virtual void onSendOK(const char* userId) = 0;

};

/////// ios上类生命编译不通过，提示重复定义错误，所以将WebSocketListener类放入WebSocket中
class WEBSocket: virtual public AsyncSocket{
public:
    // 如果sock为0，则open函数重新创建socket
    WEBSocket(WebSocketListener* listener, int sock);
    ~WEBSocket(){
        close(m_sock);
        bs_conf_destroy(&m_handshake);
        delete m_write_buffer;
    }
    
    void setListener(WebSocketListener* listener) {
        m_listener = listener;
    }
    
    void onRead();
    void onWrite() {
    }
    void onError(int error);
    void onMessage(async::message_t* msg);
    
    state_t open(const char* url,const char* sessionID);
    int sendText(const void* buf, uint32_t size);
    int sendBinary(const void* buf, uint32_t size);
    int getStatus(){
        return m_status;
    }
    int getError(){
        return m_error;
    }
    void setStatus(uint8_t status){
        m_status = status;
    }
    void setPingTime(uint64_t pingtime){
        m_pingtime = pingtime;
    }
    uint64_t getPingTime(){
        return m_pingtime;
    }
    uint64_t getPongTime(){
        return m_pongtime;
    }
    
    void setSessionId(const char* sessionId){
        strcpy(m_sessionId, sessionId);
    }
    
    void setUserId(const char* userId){
        strcpy(m_userId, userId);
    }
    
    const char* getSessionId(){
        return m_sessionId;
    }
    
    const char* getUserId(){
        return m_userId;
    }
    
    const static uint8_t OPCODE_CONTINUE    = 0x00;
    const static uint8_t OPCODE_TEXT        = 0x01;
    const static uint8_t OPCODE_BINARY      = 0x02;
    // 保留非控制帧
    const static uint8_t OPCODE_UNCONTROL   = 0X03;
    const static uint8_t OPCODE_CLOSE       = 0x08;
    const static uint8_t OPCODE_PING        = 0x09;
    const static uint8_t OPCODE_PONG        = 0x0a;
    const static uint8_t OPCODE_UNEXIST     = 0x0c;
    const static uint8_t OPCODE_OK          = 0x0d;
    const static uint8_t OPCODE_ERROR       = 0x0e;
    // 保留控制帧
    const static uint8_t OPCODE_CONTROL     = 0x0b;
    
    // 状态码
    const static uint8_t STATUS_CLOSE       = 0x00;
    const static uint8_t STATUS_OPENING     = 0x01;
    const static uint8_t STATUS_CONNECTED   = 0x02;
    const static uint8_t STATUS_ERROR       = 0x03;
    const static uint8_t STATUS_UNEXIST     = 0x04;
    
    int push(int type, const char* buf, uint32_t size);
    
    const static int SESSIONID_SIZE = 1024;
    static const int USER_SIZE      = 25;

protected:
    state_t parseRequest(const char* buf, uint32_t size);
    state_t parseResponse(const char* buf, uint32_t size);
    state_t responseHandshake();
    //int push(int type, const char* buf, uint32_t size);
    // 最多只能填mtu-8个大小
    uint32_t frame(uint8_t fin, uint8_t opcode, uint32_t mask, const char* buf, uint32_t size);
    state_t parseFrame(ws_protocal_t* protocal, const char* buf, uint32_t size);
    uint64_t protocalSize();
    uint64_t protocalBuffer(char* buffer, uint64_t size);
    //记录ping，pong的时间
    uint64_t m_pongtime;
    uint64_t m_pingtime;
protected:
    WebSocketListener*          m_listener;
    bs_conf_t                   m_handshake;
    uint8_t                     m_status;
    int                         m_error;
    
    char*                       m_write_buffer;
    cqueue_t(ws_protocal_t)     m_read_session;
    
    char                        m_sessionId[SESSIONID_SIZE];
    char                        m_userId[USER_SIZE];
};

#endif
