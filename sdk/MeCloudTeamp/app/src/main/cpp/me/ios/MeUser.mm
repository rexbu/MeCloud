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

code_user MeUser::signup(const char* username, const char* password, MeCallbackBlock callback){
    if (strncmp(username, "", 0)){
        return ME_USERNAME_ILLEGAL;
    }
    else if (strncmp(password, "", 0)){
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
    m_callback =  (__bridge MeCallbackBlock)Block_copy((__bridge void *)callback);
    
    MeCloud::shareInstance()->post(url, toString(), this);
    return ME_USER_SUCCESS;
}

code_user MeUser::login(const char* username, const char* password, MeCallbackBlock callback){
    if (strncmp(username, "", 0)){
        return ME_USERNAME_ILLEGAL;
    }
    else if (strncmp(password, "", 0)){
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
    
    m_callback =  (__bridge MeCallbackBlock)Block_copy((__bridge void *)callback);
    
    MeCloud::shareInstance()->post(url, toString(), this);
    return ME_USER_SUCCESS;
}

code_user MeUser::changePassword(const char* username, const char* oldPassword, const char* newPassword, MeCallbackBlock callback){
    if (strncmp(username, "", 0)){
        return ME_USERNAME_ILLEGAL;
    }
    else if (strncmp(oldPassword, "", 0)){
        return ME_OLDPASSWORD_ILLEGAL;
    }
    else if (strncmp(newPassword, "", 0)){
        return ME_NEWPASSWORD_ILLEGAL;
    }
    else if (strncmp(newPassword, oldPassword, 0)){
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
    m_callback =  (__bridge MeCallbackBlock)Block_copy((__bridge void *)callback);
    MeCloud::shareInstance()->post(url, toString(), this);
    return ME_USER_SUCCESS;
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
