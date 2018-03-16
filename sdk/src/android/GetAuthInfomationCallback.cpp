//
// Created by 陈冰 on 2017/8/1.
//

#include "GetAuthInfomationCallback.h"
#include "UpLoadFileCallBack.h"

GetAuthInfomationCallback::GetAuthInfomationCallback(const char *classname,
                                                     MeUploadFile *obj,
                                                     jobject thiz,
                                                     jobject jcallback)
        : MeCallback(classname, obj) {
    m_object = obj;
    meUploadFile = obj;
    this->thiz = thiz;
    this->jcallback = jcallback;
}

void GetAuthInfomationCallback::done(MeObject *obj, MeException *err,
                                     uint32_t size) {
    int errorCode = obj->intValue("errCode");
    if (isEnd && err == NULL || errorCode != 0) {// 第三步
        MeAndroidHttpFileCallBack *callback = new MeAndroidHttpFileCallBack(m_classname, meUploadFile);
        callback->lock();
        callback->setPara(thiz, jcallback);
        callback->done((MeFile *) obj, NULL);
    } else if (err == NULL && errorCode == 0) {// 第二步
        UpLoadFileCallBack *callback = new UpLoadFileCallBack(m_classname, meUploadFile, thiz,
                                                              jcallback);
        callback->lock();
        callback->setPara(thiz, jcallback);
        meUploadFile->upload(callback, obj);
    } else {
        err_log("errMsg: %s", err->errMsg());
    }
    delete this;
}

