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

MeUser::MeUser(JSONObject* obj):MeObject("User", obj)
{
    init();
}

void MeUser::init(){
    put("device", mc::device_id());
  
    // 初始化callback
    m_object = this;
#ifdef __IOS__
    m_callback = NULL;
#endif
    MeCallback::m_classname = MeObject::m_classname;
}

#pragma --mark "登出"
void MeUser::logout(){
    if (m_current_user!=NULL) {
        MeCloud::shareInstance()->clearCookie();
        char path[1024];
        FileManager::remove(SharedPreferences::path("__CurrentUser", path, sizeof(path)) );
       delete m_current_user;
       m_current_user = NULL;
    }
}

#pragma --mark "登录"
code_user MeUser::login(const char* username, const char* password, MeCallback* callbak){
    if (!strcmp(username, "")){
        return ME_USERNAME_ILLEGAL;
    }
    else if (!strcmp(password, "")){
        return ME_PASSWORD_ILLEGAL;
    }
    
    char        url[1024];
    char        userpass[256];
    char        sign[33];
    
    snprintf(url, 1024, "%slogin", MeCloud::shareInstance()->userUrl());
    sprintf(userpass, "%s/%s", username, password);
    bs_sign(userpass, sign);
    
    put("username", username);
    put("password", sign);
    
    MeCloud::shareInstance()->post(url, toString(), callbak);
    return ME_USER_SUCCESS;
}

#pragma --mark "注册"
code_user MeUser::signup(const char* username, const char* password, MeCallback* callbak){
    if (!strcmp(username, "")){
        return ME_USERNAME_ILLEGAL;
    }
    else if (!strcmp(password, "")){
        return ME_PASSWORD_ILLEGAL;
    }
    
    char        url[1024];
    char        userpass[256];
    char        sign[33];
    
    snprintf(url, 1024, "%ssignup", MeCloud::shareInstance()->userUrl());
    sprintf(userpass, "%s/%s", username, password);
    bs_sign(userpass, sign);
    
    put("username", username);
    put("password", sign);
    
    MeCloud::shareInstance()->post(url, toString(), callbak);
    return ME_USER_SUCCESS;
}

#pragma --mark "修改密码"
code_user MeUser::changePassword(const char* username, const char* oldPassword, const char* newPassword, MeCallback* callbak){
    if (strncmp(username, "", 0)){
        return ME_USERNAME_ILLEGAL;
    } else if (strncmp(oldPassword, "", 0)){
        return ME_OLDPASSWORD_ILLEGAL;
    } else if (strncmp(newPassword, "", 0)){
        return ME_NEWPASSWORD_ILLEGAL;
    } else if (strncmp(newPassword, oldPassword, 0)){
        return ME_PASSWORD_SAME;
    }
    
    char        url[1024];
    char        oldpass[256];
    char        oldsign[33];
    char        newpass[256];
    char        newsign[33];
    
    snprintf(url, 1024, "%smodifyPwd", MeCloud::shareInstance()->userUrl());
    sprintf(oldpass, "%s/%s", username, oldPassword);
    bs_sign(oldpass, oldsign);
    sprintf(newpass, "%s/%s", username, newPassword);
    bs_sign(newpass, newsign);
    put("oldPwd", oldsign);
    put("newPwd", newsign);
    MeCloud::shareInstance()->post(url, toString(), callbak);
    return ME_USER_SUCCESS;
}

code_user MeUser::changePassword(const char* username, const char* newPassword, MeCallback* callbak){
    if (strncmp(username, "", 0)){
        return ME_USERNAME_ILLEGAL;
    } else if (strncmp(newPassword, "", 0)){
        return ME_NEWPASSWORD_ILLEGAL;
    }
    
    char        url[1024];
    char        newpass[256];
    char        newsign[33];
    
    snprintf(url, 1024, "%smodifyPwd", MeCloud::shareInstance()->userUrl());
    sprintf(newpass, "%s/%s", username, newPassword);
    bs_sign(newpass, newsign);
    put("newPwd", newsign);
    MeCloud::shareInstance()->post(url, toString(), callbak);
    return ME_USER_SUCCESS;
}

const char* MeUser::encodePassword(const char* username, const char* password) {
  char userpass[256];
  char *sign = new char[33];
  sprintf(userpass, "%s/%s", username, password);
  bs_sign(userpass, sign);
  return sign;
}

const char* MeUser::device() {
  return mc::device_id();
}

MeUser* MeUser::currentUser() {
    mc::device_id();
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

void MeUser::saveLoginUser(MeObject *object) {
    m_current_user = new MeUser(object);
    m_current_user->saveLocalCache();
}

void MeUser::saveLocalCache() {
    if (!stringValue("_id")) {
        return;
    }
    
    char path[1024];
    FileManager::remove(SharedPreferences::path("__CurrentUser", path, sizeof(path)) );
    Crypt crypt;
    const char* str = toString();
    int64_t crysize = crypt.encryptByCompress((byte*)str, (uint32_t)strlen(str));
    FileManager::write(path, crypt.bytes(), crysize);
}
