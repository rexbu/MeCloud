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
    code_user changePassword(const char* username, const char* newPassword, MeCallbackBlock callback);
#endif
    
    code_user signup(const char* username, const char* password, MeCallback* callback);
    code_user login(const char* username, const char* password, MeCallback* callback);
    code_user changePassword(const char* username, const char* oldPassword, const char* newPassword, MeCallback* callback);
    code_user changePassword(const char* username, const char* newPassword, MeCallback* callback);
    
    void logout();
  
    static const char* encodePassword(const char* username, const char* password);
    static const char* device();
  
    static MeUser* currentUser();
    void saveLocalCache();
    static void saveLoginUser(MeObject* object);

#ifdef __IOS__
    virtual void done(MeObject* obj, MeException* err, uint32_t size = 1);
#else
    virtual void done(MeObject* obj, MeException* err, uint32_t size = 1){};
#endif
protected:
    void init();
    
    static MeUser*      m_current_user;
    static const char*  m_version;
#ifdef __IOS__
    MeCallbackBlock     m_callback;
#else
#endif
};

#endif
