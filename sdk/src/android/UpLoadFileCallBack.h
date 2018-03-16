//
// Created by 陈冰 on 2017/8/1.
//


#include "jni.h"
#include <MeCallback.h>
#include <MeUploadFile.h>
#include "MeAndroidHttpFileCallBack.h"

class UpLoadFileCallBack : public MeAndroidHttpFileCallBack {

public:
    MeUploadFile* meUploadFile = NULL;
    jobject thiz = NULL;
    jobject jcallback = NULL;
    UpLoadFileCallBack(const char *classname, MeUploadFile* obj, jobject thiz, jobject jcallback);
    virtual void done(MeFile *file, MeException *err,
                      uint32_t size);
};


