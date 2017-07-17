/**
 * file :	MeUser.mm
 * author :	Rex
 * create :	2017-07-07 17:08
 * func : 
 * history:
 */

#include "bs.h"
#include "MeUser.h"
#import <Foundation/Foundation.h>

void MeUser::signup(const char* username, const char* password, MeCallbackBlock callback){
    char        url[1024];
    char        userpass[256];
    char        sign[33];
    
    snprintf(url, 1024, "%ssignup", MeCloud::shareInstance()->userUrl());
    sprintf(userpass, "%s/%s", username, password);
    bs_sign(userpass, sign);
    
    put("username", username);
    put("password", sign);
    m_callback =  (__bridge MeCallbackBlock)Block_copy((__bridge void *)callback);
    
    MeCloud::shareInstance()->post(url, toString(), this);
}

void MeUser::login(const char* username, const char* password, MeCallbackBlock callback){
    char        url[1024];
    char        userpass[256];
    char        sign[33];
    
    snprintf(url, 1024, "%slogin", MeCloud::shareInstance()->userUrl());
    sprintf(userpass, "%s/%s", username, password);
    bs_sign(userpass, sign);
    
    put("username", username);
    put("password", sign);
    
    m_callback =  (__bridge MeCallbackBlock)Block_copy((__bridge void *)callback);
    
    MeCloud::shareInstance()->post(url, toString(), this);
}

#pragma --mark "回调"
void MeUser::done(MeObject* obj, MeException* err, uint32_t size){
    // 写currentUser
    if (err == NULL) {
        // 存储CurrentUser
        m_current_user = this;
        saveLocalCache();
    }
    
    if (m_callback!=nil) {
        // 在主线程中回调
        dispatch_async(dispatch_get_main_queue(), ^{
            m_callback(obj, err, size);
            Block_release(m_callback);
            m_callback = nil;
        });
    }
}
