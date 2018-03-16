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

code_user MeUser::signup(const char* username, const char* password, MeCallbackBlock callback) {
    m_callback =  (__bridge MeCallbackBlock)Block_copy((__bridge void *)callback);
    return signup(username, password, this);
}

code_user MeUser::login(const char* username, const char* password, MeCallbackBlock callback) {
    m_callback =  (__bridge MeCallbackBlock)Block_copy((__bridge void *)callback);
    return login(username, password, this);
}

code_user MeUser::changePassword(const char* username, const char* newPassword, MeCallbackBlock callback){
    m_callback =  (__bridge MeCallbackBlock)Block_copy((__bridge void *)callback);
    return changePassword(username, newPassword, this);
}

#pragma --mark "回调"
void MeUser::done(MeObject *obj, MeException *err, uint32_t size) {
    // 写currentUser
    if (err == NULL) {
        // 存储CurrentUser
        saveLoginUser(obj);
    }
    
    if (m_callback != nil) {
        // 在主线程中回调
        dispatch_sync(dispatch_get_main_queue(), ^{
            m_callback(obj, err, size);
            Block_release(m_callback);
            m_callback = nil;
        });
    }
}
