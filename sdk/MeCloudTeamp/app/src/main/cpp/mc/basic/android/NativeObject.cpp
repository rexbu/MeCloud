/**
 * file :	NativeObject.cpp
 * author :	bushaofeng
 * create :	2016-08-24 14:53
 * func : 
 * history:
 */

#include "NativeObject.h"
#include "bs.h"

// JavaVM*	g_jvm = NULL;
// char g_package_name[256] = {0};

// jint JNI_OnLoad(JavaVM *vm, void *reserved) {  
//     void *env = NULL;  
//     //LOGI("JNI_OnLoad");  
//     if (vm->GetEnv(&env, JNI_VERSION_1_6) != JNI_OK) {  
//         err_log("ERROR: GetEnv failed");  
//         return -1;
//     }

//     FILE* fp = fopen("/proc/self/cmdline", "r");
//     fread(g_package_name, sizeof(g_package_name), 1, fp);
//     /*
//     if (strcmp(g_package_name, "com.visionin.demo")!=0)
//     {
//         return -1;
//     }
//     */
//     fclose(fp);

//     g_jvm = vm;
//     err_log("g_jvm: %llu", (unsigned long long)g_jvm);

//     return JNI_VERSION_1_6;
// }

// NativeObject::NativeObject(const char* className){
// 	//jvm->AttachCurrentThread(&m_jenv, NULL);
// 	g_jvm->GetEnv(&m_jenv, JNI_VERSION_1_6);

// 	m_class = m_jenv->FindClass(className);
// 	jmethodID construction_id = m_jenv->GetMethodID(m_class,  
//             "<init>", "()V");
// 	m_object = m_jenv->NewObject(m_class, construction_id);
// }