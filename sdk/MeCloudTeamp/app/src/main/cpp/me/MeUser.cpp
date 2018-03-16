/**
 * file :	MeUser.cpp
 * author :	Rex
 * create :	2016-09-21 15:14
 * func : 
 * history:
 */

#include "MeUser.h"
#include "SharedPreferences.h"
#include "McFile.h"
#include "McZip.h"
#include "McDevice.h"

using namespace mc;
MeUser* MeUser::m_current_user = NULL;
const char* MeUser::m_version = "2.10";

MeUser::MeUser():
MeObject("User"){
    init();
}

MeUser::MeUser(JSONObject* obj):
MeObject("User", obj)
{
    init();
}

void MeUser::init(){
    put("device", mc::device_id());
    put("version", m_version);
    
    // 初始化callback
    m_object = this;
#ifdef __IOS__
    m_callback = NULL;
#endif
    MeCallback::m_classname = MeObject::m_classname;
}

#pragma --mark "登录"
void MeUser::logout(){
    if (m_current_user!=NULL) {
        char path[1024];
        FileManager::remove(SharedPreferences::path("__CurrentUser", path, sizeof(path)) );
        delete m_current_user;
        m_current_user = NULL;
    }
}

#pragma --mark "注册"
/*
// 验证码注册
void MeUser::signin(const char* username, const char* password, const char* authcode, MeCallback_func callback){
    char url[1024];
    snprintf(url, 1024, "%ssignin", MeCloud::shareInstance()->userUrl());
    put("username", username);
    put("authcode", authcode);
    m_callback = callback;
    MeCloud::shareInstance()->post(url, toString(), this);
}

#pragma --mark "回调"
void MeUser::done(MeObject* obj, MeException* err, uint32_t size){
    // 写currentUser
    if (err!=NULL) {
        m_callback(obj, err, size);
        return;
    }
    
    // 存储CurrentUser
    m_current_user = this;
    char path[1024];
    FileManager::remove(SharedPreferences::path("__CurrentUser", path, sizeof(path)) );
    Crypt crypt;
    const char* str = toString();
    int64_t crysize = crypt.encrypt((byte*)str, (uint32_t)strlen(str));
    FileManager::write(path, crypt.bytes(), crysize);
    
    m_callback(obj, err, size);
}
*/

MeUser* MeUser::currentUser(){
    if (m_current_user == NULL) {
        SharedPreferences preference("__CurrentUser");
        SharedPreferences cookiePreference("__CurrentCookie");
        if (preference.exist() && cookiePreference.exist()) {
            m_current_user = new MeUser(&preference);
            JSONObject *cookie = new JSONObject(&cookiePreference);
            MeCloud::shareInstance()->httpSession()->setCookie(cookie->stringValue("Cookie"));
            delete cookie;
            return m_current_user;
        }
        
        return NULL;
    }
    
    return m_current_user;
}

void MeUser::saveLocalCache(){
    char path[1024];
    FileManager::remove(SharedPreferences::path("__CurrentUser", path, sizeof(path)) );
    Crypt crypt;
    const char* str = toString();
    int64_t crysize = crypt.encrypt((byte*)str, (uint32_t)strlen(str));
    FileManager::write(path, crypt.bytes(), crysize);
}
