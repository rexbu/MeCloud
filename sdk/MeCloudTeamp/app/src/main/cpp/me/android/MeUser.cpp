/**
 * file :	MeUser.cpp
 * author :	Rex
 * create :	2017-07-18 15:15
 * func : 
 * history:
 */

#include "MeUser.h"

 void MeUser::signup(const char* username, const char* password, MeCallback_func callback){
     // char url[1024];
     // snprintf(url, 1024, "%ssignin", MeCloud::shareInstance()->userUrl());
     // put("username", username);
     // MeCloud::shareInstance()->post(url, toString(), this);
 }

 #pragma --mark "回调"
 void MeUser::done(MeObject* obj, MeException* err, uint32_t size){
     // 写currentUser
     // if (err!=NULL) {
     //     m_callback(obj, err, size);
     //     return;
     // }
     
     // // 存储CurrentUser
     // m_current_user = this;
     // char path[1024];
     // FileManager::remove(SharedPreferences::path("__CurrentUser", path, sizeof(path)) );
     // Crypt crypt;
     // const char* str = toString();
     // int64_t crysize = crypt.encrypt((byte*)str, (uint32_t)strlen(str));
     // FileManager::write(path, crypt.bytes(), crysize);
     
     // m_callback(obj, err, size);
 }