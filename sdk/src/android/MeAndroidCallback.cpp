/**
 * file :	MeAndroidCallback.cpp
 * author :	Rex
 * create :	2017-07-19 21:00
 * func : 
 * history:
 */
#include <jni.h>
#include <Me.h>
#include "MeAndroidCallback.h"

#ifndef EXPORT_API
#define EXPORT_API __attribute__((visibility("default")))
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern jfieldID g_ptr;
extern jclass g_exception_class;
extern jmethodID g_exception_init;
extern jmethodID g_mecloud_callback;
extern jmethodID g_mecloud_callbackList;

extern void globalMeObject(JNIEnv *env);

extern void global_exception(JNIEnv *env);

extern const char *getJclassName(JNIEnv *env, jclass clazz);
#ifdef __cplusplus
}
#endif

MeObjectCallback::MeObjectCallback(const char *classname, MeObject *object): MeCallback(classname, object) {

}

MeObjectCallback::MeObjectCallback(jobject thiz, jobject callback) {
    m_thiz = thiz;
    m_callback = callback;
}

void MeObjectCallback::setPara(jobject thiz, jobject callback) {
    m_thiz = thiz;
    m_callback = callback;
}

void MeObjectCallback::setIsMeListCallback(bool_t isMeListCallback) {
    this->isMeListCallback = isMeListCallback;
}

void MeObjectCallback::done(MeObject *obj, MeException *err, uint32_t size) {
    extern JavaVM *g_jvm;
    JNIEnv *env;
    int state = g_jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    switch (state) {
        case JNI_EDETACHED:
            g_jvm->AttachCurrentThread(&env, NULL);
            break;
        case JNI_EVERSION:
            err_log("%s", "Version not support!")
            return;
        case JNI_OK:
            break;
        default:break;
    }
    err_log("MeObjectCallback::done -> callback!!!!!!!!");
    if (g_exception_class == NULL) {
        global_exception(env);
    }

    if (err != NULL) {
        jobject errObj = env->AllocObject(g_exception_class);
        jstring errMsg = env->NewStringUTF(err->errMsg());
        jstring info = env->NewStringUTF(err->info());
        env->CallVoidMethod(errObj, g_exception_init, err->errCode(), errMsg, info);
        if (isMeListCallback) {
            env->CallVoidMethod(m_thiz, g_mecloud_callbackList, NULL, m_callback, errObj);
        } else {
            env->CallVoidMethod(m_thiz, g_mecloud_callback, (jlong) 0, m_callback, errObj);
        }
    } else {
        if (isMeListCallback) {
            jlong objptrs[size];
            jlongArray jobjptrs = env->NewLongArray(size);
            for (int i = 0; i < size; ++i) {
                JSONObject *object = new JSONObject(&obj[i], false);
                objptrs[i] = (jlong) object;
            }
            env->SetLongArrayRegion(jobjptrs, 0, size, objptrs);
            env->CallVoidMethod(m_thiz, g_mecloud_callbackList, jobjptrs, m_callback, NULL);
        } else {
            JSONObject *object = new JSONObject(obj, false);
            env->CallVoidMethod(m_thiz, g_mecloud_callback, (jlong)object, m_callback, NULL);
        }
    }
    unLock();

    env->DeleteGlobalRef(m_thiz);
    env->DeleteGlobalRef(m_callback);
    g_jvm->DetachCurrentThread();
    delete this;
}

MeUserCallback::MeUserCallback(MeUser *meUser): MeObjectCallback("User", meUser) {

}

void MeUserCallback::done(MeObject *obj, MeException *err, uint32_t size) {
    extern JavaVM *g_jvm;
    JNIEnv *env;
    int state = g_jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    switch (state) {
        case JNI_EDETACHED:
            g_jvm->AttachCurrentThread(&env, NULL);
            break;
        case JNI_EVERSION:
            return;
        case JNI_OK:
            break;
        default:break;
    }

    if (g_exception_class == NULL) {
        global_exception(env);
    }

    if (err != NULL) {
        jobject errObj = env->AllocObject(g_exception_class);
        jstring errMsg = env->NewStringUTF(err->errMsg());
        jstring info = env->NewStringUTF(err->info());
        env->CallVoidMethod(errObj, g_exception_init, err->errCode(), errMsg, info);
        env->CallVoidMethod(m_thiz, g_mecloud_callback, m_callback, errObj);
    } else {
        MeUser *meUser = new MeUser(obj);
        meUser->saveLocalCache();
        env->CallVoidMethod(m_thiz, g_mecloud_callback, (jlong)meUser, m_callback, NULL);
    }
    unLock();

    env->DeleteGlobalRef(m_thiz);
    env->DeleteGlobalRef(m_callback);
    g_jvm->DetachCurrentThread();
    delete this;
}

