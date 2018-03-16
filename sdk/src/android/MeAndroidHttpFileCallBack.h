//
//  MeIOSDownloadCallBack.h
//  MeCloud
//
//  Created by super on 2017/7/25.
//  Copyright © 2017年 Rex. All rights reserved.
//

#ifndef	__MEANDROIDDOWNLOADCALLBACK_H_
#define	__MEANDROIDDOWNLOADCALLBACK_H_

#include "jni.h"
#include "MeHttpFileCallback.h"

class MeAndroidHttpFileCallBack: public MeHttpFileCallback {
public:
    MeAndroidHttpFileCallBack(const char *classname = NULL, MeFile *file = NULL);

    virtual void done(MeFile *file, MeException *err, uint32_t size = 1);

    virtual void progress(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write);

    void setPara(jobject thiz, jobject callback);
protected:
    jobject m_thiz;
    jobject m_callback;

    void getCallbackMethodId(JNIEnv *env) const;
};

class MeAndroidHttpDataCallBack: public MeHttpDataCallback {
public:
    MeAndroidHttpDataCallBack(const char *classname = NULL, MeObject* json = NULL);

    virtual void done(MeObject* json, MeException *err, uint32_t size = 1);

    virtual void progress(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write);

    void setPara(jobject thiz, jobject callback);
protected:
    jobject m_thiz;
    jobject m_callback;

    void getCallbackMethodId(JNIEnv *env) const;
};

#endif
