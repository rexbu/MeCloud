//
// Created by 陈冰 on 2017/7/25.
//

#include "jni.h"
#include "MeAndroidHttpFileCallBack.h"
#include <MeFile.h>

#ifdef __cplusplus
extern "C" {
#endif
jmethodID fileDoneId;
jmethodID fileProgressId;
jmethodID dataDoneId;
jmethodID dataProgressId;
extern jclass g_exception_class;
extern jmethodID g_exception_init;
extern void global_exception(JNIEnv *env);
#ifdef __cplusplus
};
#endif

MeAndroidHttpFileCallBack::MeAndroidHttpFileCallBack(const char *classname,
                                             MeFile *file)
        : MeHttpFileCallback(classname, file) {
}

void MeAndroidHttpFileCallBack::setPara(jobject thiz, jobject callback) {
    m_thiz = thiz;
    m_callback = callback;
}

void MeAndroidHttpFileCallBack::done(MeFile *file, MeException *err,
                                 uint32_t size) {
    extern JavaVM *g_jvm;
    JNIEnv *env;
    int state_env = g_jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    switch (state_env) {
        case JNI_EDETACHED:
            g_jvm->AttachCurrentThread(&env, NULL);
            break;
        case JNI_EVERSION:
            err_log("%s", "Version not support!")
            return;
        case JNI_OK:
            break;
        default:
            break;
    }
    getCallbackMethodId(env);
    if (m_callback) {
        if (err != NULL) {
            if (g_exception_init == NULL) {
                global_exception(env);
            }
            jobject errObj = env->AllocObject(g_exception_class);
            jstring errMsg = env->NewStringUTF(err->errMsg());
            jstring info = env->NewStringUTF(err->info());
            env->CallVoidMethod(errObj, g_exception_init, err->errCode(), errMsg,
                                info);
            env->CallVoidMethod(m_thiz, fileDoneId,(jlong) 0, m_callback, errObj);
            err_log("MeAndroidHttpFileCallBack::done -> %s", "down Error");
        } else {
            JSONObject *object = new JSONObject(file, false);
            env->CallVoidMethod(m_thiz, fileDoneId,(jlong) object, m_callback, NULL);
            err_log("MeAndroidHttpFileCallBack::done -> %s", "down success");
        }
    }
    unLock();

    env->DeleteGlobalRef(m_thiz);
    env->DeleteGlobalRef(m_callback);
    g_jvm->DetachCurrentThread();
    delete this;
}

void MeAndroidHttpFileCallBack::progress(uint64_t writen, uint64_t total_writen,
                                     uint64_t total_expect_write) {
    extern JavaVM *g_jvm;
    JNIEnv *env;
    int state_env = g_jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    switch (state_env) {
        case JNI_EDETACHED:
            g_jvm->AttachCurrentThread(&env, NULL);
            break;
        case JNI_EVERSION:
            err_log("%s", "Version not support!")
            return;
        case JNI_OK:
            break;
        default:
            break;
    }
    getCallbackMethodId(env);
    if (m_callback) {
        env->CallVoidMethod(m_thiz, fileProgressId, m_callback, writen, total_writen,
                            total_expect_write);
    }
    g_jvm->DetachCurrentThread();
}

void MeAndroidHttpFileCallBack::getCallbackMethodId(JNIEnv *env) const {
    jclass thizClass = env->GetObjectClass(m_thiz);
    if (fileDoneId == NULL) {
        fileDoneId =
                env->GetMethodID(
                        thizClass,
                        "doneCallback",
                        "(JLcom/rex/mecloud/HttpFileCallback;Lcom/rex/mecloud/MeException;)V");
    }
    if (fileProgressId == NULL) {
        fileProgressId = env->GetMethodID(
                thizClass, "progressCallback",
                "(Lcom/rex/mecloud/HttpFileCallback;JJJ)V");
    }
    env->DeleteLocalRef(thizClass);
}

MeAndroidHttpDataCallBack::MeAndroidHttpDataCallBack(const char *classname,
                                                     MeObject *json)
        : MeHttpDataCallback() {
}

void MeAndroidHttpDataCallBack::setPara(jobject thiz, jobject callback) {
    m_thiz = thiz;
    m_callback = callback;
}

void MeAndroidHttpDataCallBack::done(MeObject* obj, MeException *err,
                                     uint32_t size) {
    extern JavaVM *g_jvm;
    JNIEnv *env;
    int state_env = g_jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    switch (state_env) {
        case JNI_EDETACHED:
            g_jvm->AttachCurrentThread(&env, NULL);
            break;
        case JNI_EVERSION:
            err_log("%s", "Version not support!")
            return;
        case JNI_OK:
            break;
        default:
            break;
    }
    err_log("MeAndroidHttpFileCallBack::done -> %s", "down 1")
    getCallbackMethodId(env);
    err_log("MeAndroidHttpFileCallBack::done -> %s", "down 2")
    if (m_callback) {
        if (err != NULL) {
            if (g_exception_init == NULL) {
                global_exception(env);
            }
            jobject errObj = env->AllocObject(g_exception_class);
            jstring errMsg = env->NewStringUTF(err->errMsg());
            jstring info = env->NewStringUTF(err->info());
            env->CallVoidMethod(errObj, g_exception_init, err->errCode(), errMsg,
                                info);
            env->CallVoidMethod(m_thiz, dataDoneId,(jlong) 0, m_callback, errObj);
        } else {
            JSONObject *object = new JSONObject(obj, false);
            env->CallVoidMethod(m_thiz, dataDoneId,(jlong)object, m_callback, NULL);
        }
    }
    unLock();

    env->DeleteGlobalRef(m_thiz);
    env->DeleteGlobalRef(m_callback);
    g_jvm->DetachCurrentThread();
    delete this;
}

void MeAndroidHttpDataCallBack::progress(uint64_t writen, uint64_t total_writen,
                                         uint64_t total_expect_write) {
    extern JavaVM *g_jvm;
    JNIEnv *env;
    int state_env = g_jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    switch (state_env) {
        case JNI_EDETACHED:
            g_jvm->AttachCurrentThread(&env, NULL);
            break;
        case JNI_EVERSION:
            return;
        case JNI_OK:
            break;
        default:
            break;
    }
    getCallbackMethodId(env);
    if (m_callback) {
        env->CallVoidMethod(m_thiz, dataProgressId, m_callback, (jlong)writen, (jlong) total_writen,
                            (jlong)total_expect_write);
    }
    g_jvm->DetachCurrentThread();
}

void MeAndroidHttpDataCallBack::getCallbackMethodId(JNIEnv *env) const {
    jclass thizClass = env->GetObjectClass(m_thiz);
    if (dataDoneId == NULL) {
        dataDoneId =
                env->GetMethodID(
                        thizClass,
                        "doneCallback",
                        "(JLcom/rex/mecloud/HttpDataCallback;Lcom/rex/mecloud/MeException;)V");
    }
    if (dataProgressId == NULL) {
        dataProgressId = env->GetMethodID(
                thizClass,
                "progressCallback",
                "(Lcom/rex/mecloud/HttpDataCallback;JJJ)V");
    }
    env->DeleteLocalRef(thizClass);
}

