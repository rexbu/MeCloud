//
// Created by 陈冰 on 2017/8/1.
//

#include "UpLoadFileCallBack.h"
#include "GetAuthInfomationCallback.h"

UpLoadFileCallBack::UpLoadFileCallBack(const char *classname,
                                       MeUploadFile *obj,
                                       jobject thiz,
                                       jobject jcallback)
        : MeAndroidHttpFileCallBack(classname, obj) {
    meUploadFile = obj;
    this->thiz = thiz;
    this->jcallback = jcallback;
}

void UpLoadFileCallBack::done(MeFile *file, MeException *err,
                              uint32_t size) {
    if (err == NULL) {
        GetAuthInfomationCallback *callback = new GetAuthInfomationCallback(m_classname,
                                                                            meUploadFile, thiz,
                                                                            jcallback);
        callback->isEnd = (jboolean) true;
        callback->lock();
        meUploadFile->uploadFileInfomation(callback);
    } else {
        err_log("errMsg: %s", err->errMsg());
    }
    delete this;
}
