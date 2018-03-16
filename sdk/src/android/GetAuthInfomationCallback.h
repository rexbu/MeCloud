//
// Created by 陈冰 on 2017/8/1.
//

#ifndef MECLOUDTEAMP_GETAUTHINFOMATIONCALLBACK_H
#define MECLOUDTEAMP_GETAUTHINFOMATIONCALLBACK_H

#include "jni.h"
#include <MeCallback.h>
#include <MeUploadFile.h>
#include "MeAndroidHttpFileCallBack.h"

class GetAuthInfomationCallback : public MeCallback {

public:
    MeUploadFile *meUploadFile = NULL;
    jobject thiz = NULL;
    jobject jcallback = NULL;
    jboolean isEnd = false;
    GetAuthInfomationCallback(const char *classname, MeUploadFile *obj, jobject thiz,
                              jobject jcallback);

    virtual void done(MeObject *obj, MeException *err, uint32_t size);
};


#endif //MECLOUDTEAMP_GETAUTHINFOMATIONCALLBACK_H
