/**
 * file :	MeAndroidCallback.cpp
 * author :	Rex
 * create :	2017-07-19 21:00
 * func : 
 * history:
 */

#include "MeAndroidCallback.h"
#include "Me.h"

extern jclass       g_meobject_class;
extern jfieldID     g_meobject_ptr;
extern jmethodID    g_meobject_callback;
extern jclass      	g_exception_class;
extern jmethodID   	g_exception_init;

extern void globalJavaClass(JNIEnv* env);

MeObjectCallback::MeObjectCallback(jobject thiz, jobject callback){
	m_thiz = thiz;
	m_callback = callback;
}

void MeObjectCallback::setPara(jobject thiz, jobject callback){
	m_thiz = thiz;
	m_callback = callback;
}

void MeObjectCallback::done(MeObject* obj, MeException* err, uint32_t size){
	extern JavaVM*  g_jvm;
    JNIEnv*     	env;
    g_jvm->AttachCurrentThread(&env, NULL);

    err_log("callback!!!!!!!!");
    if (g_exception_class==NULL)
    {
    	globalJavaClass(env);
    }

    if (err != NULL)
    {
    	jobject errObj =env->AllocObject(g_exception_class);
    	jstring errMsg = env->NewStringUTF(err->errMsg());
    	jstring info = env->NewStringUTF(err->info());
    	env->CallVoidMethod(errObj, g_exception_init, err->errCode(), errMsg, info);
    	env->CallVoidMethod(m_thiz, g_meobject_callback, m_callback, errObj);
    }
    else{
    	env->CallVoidMethod(m_thiz, g_meobject_callback, m_callback, NULL);	
    }

	unLock();

	env->DeleteGlobalRef(m_thiz);
	env->DeleteGlobalRef(m_callback);
}