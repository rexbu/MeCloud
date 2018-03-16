/**
 * file :	MeUser.h
 * author :	Rex
 * create :	2016-09-21 15:14
 * func : 
 * history:
 */

#ifndef	__MEUSER_H_
#define	__MEUSER_H_

#include "MeObject.h"
#include "MeError.h"

class MeUser: public MeObject, public MeCallback{
public:
    MeUser();
    MeUser(JSONObject* obj);
    
    // 验证码注册
#ifdef __IOS__
    code_user signup(const char* username, const char* password, MeCallbackBlock callback);
    code_user login(const char* username, const char* password, MeCallbackBlock callback);
    code_user changePassword(const char* username, const char* oldPassword, const char* newPassword, MeCallbackBlock callback);
#else
    void signup(const char* username, const char* password, MeCallback_func callback);
    // 登录
    void login(const char* username, const char* password, MeCallback* callback);
#endif
    
    void logout();
    
    static MeUser* currentUser();
    
    virtual void done(MeObject* obj, MeException* err, uint32_t size = 1);
    
protected:
    void init();
    void saveLocalCache();
    
    static MeUser*      m_current_user;
    static const char*  m_version;
#ifdef __IOS__
    MeCallbackBlock     m_callback;
#else
#endif
};

#endif
