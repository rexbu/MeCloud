/**
 * file :	android_mecloud.cpp
 * author :	Rex
 * create :	2017-07-17 18:50
 * func : 
 * history:
 */

#include <jni.h>
#include <iostream>
#include "../../mc/bs/bs.h"
#include "MeAndroidCallback.h"
#include "../MeObject.h"
#include "../../mc/basic/McBasic.h"

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

jlong create(JNIEnv* env, jobject jo, jstring className);
void destroy(JNIEnv* env, jobject jo);
void save(JNIEnv* env, jobject thiz, jobject jcallback);

void putString(JNIEnv* env, jobject jo, jstring jkey, jstring jvalue);
void putDouble(JNIEnv* env, jobject jo, jstring jkey, jdouble jvalue);
void putInt(JNIEnv* env, jobject jo, jstring jkey, jint jvalue);
void putLong(JNIEnv* env, jobject jo, jstring jkey, jlong jvalue);
void putFloat(JNIEnv* env, jobject jo, jstring jkey, jfloat jvalue);
void putBoolean(JNIEnv* env, jobject jo, jstring jkey, jboolean jvalue);
void putObject(JNIEnv* env, jobject jo, jstring jkey, jobject jvalue);

jstring stringValue(JNIEnv* env, jobject jo, jstring jkey);
jdouble doubleValue(JNIEnv* env, jobject jo, jstring jkey);
jint intValue(JNIEnv* env, jobject jo, jstring jkey);
jlong longValue(JNIEnv* env, jobject jo, jstring jkey);
jfloat floatValue(JNIEnv* env, jobject jo, jstring jkey);
jboolean booleanValue(JNIEnv* env, jobject jo, jstring jkey);
jlong jsonValue(JNIEnv* env, jobject jo, jstring jkey);
jlong arrayValue(JNIEnv* env, jobject jo, jstring jkey);

#ifdef __cplusplus
}
#endif

jclass g_meobject_class = NULL;
jfieldID g_meobject_ptr = NULL;
jmethodID g_meobject_callback = NULL;

jclass g_exception_class = NULL;
jmethodID g_exception_init = NULL;

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

void globalJavaClass(JNIEnv* env) {
  g_meobject_class = env->FindClass("com/rex/mecloud/MeObject");
  g_meobject_ptr = env->GetFieldID(g_meobject_class, "objectPtr", "J");
  g_meobject_callback = env->GetMethodID(
      g_meobject_class, "callback",
      "(Lcom/rex/mecloud/MeCallback;Lcom/rex/mecloud/MeException;)V");

  g_exception_class = env->FindClass("com/rex/mecloud/MeException");
  g_exception_init = env->GetMethodID(
      g_exception_class, "<init>", "(ILjava/lang/String;Ljava/lang/String;)V");
}

jlong create(JNIEnv* env, jobject jo, jstring jclassName) {
  const char* className = env->GetStringUTFChars(jclassName, NULL);

  MeObject* obj = new MeObject(className);
  if (g_meobject_class == NULL) {
    globalJavaClass(env);
  }
  env->ReleaseStringUTFChars(jclassName, className);
  return (jlong) obj;
}

void setObjectPtr(JNIEnv* env,jobject thiz, jstring jclassname, jstring jfieldname, jstring jsign) {
  const char* className = env->GetStringUTFChars(jclassname, NULL);
  const char* fieldName = env->GetStringUTFChars(jfieldname, NULL);
  const char* sign = env->GetStringUTFChars(jsign, NULL);
  g_meobject_ptr = env->GetFieldID(env->FindClass(className), fieldName, sign);
  env->ReleaseStringUTFChars(jclassname, className);
  env->ReleaseStringUTFChars(jfieldname, fieldName);
  env->ReleaseStringUTFChars(jsign, sign);
}

void destroy(JNIEnv* env, jobject jo) {
  JSON* obj = (JSON*) env->GetLongField(jo, g_meobject_ptr);
  delete obj;
}

void save(JNIEnv* env, jobject thiz, jobject jcallback) {
  //传入的object组要做全局引用，否则函数返回后会被释放
  jobject global_thiz = env->NewGlobalRef(thiz);
  jobject global_callback = env->NewGlobalRef(jcallback);

  ReferenceCache* cache = ReferenceCache::shareInstance();
  MeObjectCallback* callback = (MeObjectCallback*) cache->get();

  if (callback == NULL) {
    callback = new MeObjectCallback(global_thiz, global_callback);
    callback->lock();
    cache->add(callback);
  } else {
    callback->lock();
    callback->setPara(global_thiz, global_callback);
  }

  MeObject* obj = (MeObject*) env->GetLongField(thiz, g_meobject_ptr);
  obj->save(callback);
}

void putString(JNIEnv* env, jobject jo, jstring jkey, jstring jvalue) {
  const char* key = env->GetStringUTFChars(jkey, NULL);
  const char* value = env->GetStringUTFChars(jvalue, NULL);

  MeObject* obj = (MeObject*) env->GetLongField(jo, g_meobject_ptr);
  obj->put(key, value);

  env->ReleaseStringUTFChars(jkey, key);
  env->ReleaseStringUTFChars(jvalue, value);
}

void putDouble(JNIEnv* env, jobject jo, jstring jkey, jdouble jvalue) {
  const char* key = env->GetStringUTFChars(jkey, NULL);

  MeObject* obj = (MeObject*) env->GetLongField(jo, g_meobject_ptr);
  obj->put(key, jvalue);

  env->ReleaseStringUTFChars(jkey, key);
}

void putInt(JNIEnv* env, jobject jo, jstring jkey, jint jvalue) {
  const char* key = env->GetStringUTFChars(jkey, NULL);

  MeObject* obj = (MeObject*) env->GetLongField(jo, g_meobject_ptr);
  obj->put(key, jvalue);

  env->ReleaseStringUTFChars(jkey, key);
}

void putLong(JNIEnv* env, jobject jo, jstring jkey, jlong jvalue) {
  const char* key = env->GetStringUTFChars(jkey, NULL);

  MeObject* obj = (MeObject*) env->GetLongField(jo, g_meobject_ptr);
  obj->put(key, jvalue);

  env->ReleaseStringUTFChars(jkey, key);
}

void putFloat(JNIEnv* env, jobject jo, jstring jkey, jfloat jvalue) {
  const char* key = env->GetStringUTFChars(jkey, NULL);

  MeObject* obj = (MeObject*) env->GetLongField(jo, g_meobject_ptr);
  obj->put(key, jvalue);

  env->ReleaseStringUTFChars(jkey, key);
}

void putBoolean(JNIEnv* env, jobject jo, jstring jkey, jboolean jvalue) {
  const char* key = env->GetStringUTFChars(jkey, NULL);
  const bool value = jvalue;
  MeObject* obj = (MeObject*) env->GetLongField(jo, g_meobject_ptr);
  obj->put(key, value);

  env->ReleaseStringUTFChars(jkey, key);
}

//void putObject(JNIEnv* env, jobject jo, jstring jkey, jobject jvalue){
//    const char* key = env->GetStringUTFChars(jkey, NULL);
//    MeObject* obj = (MeObject*)env->GetLongField(jo, g_meobject_ptr);
//    obj->put(key, (bool) jvalue);
//
//    env->ReleaseStringUTFChars(jkey, key);
//}

jstring stringValue(JNIEnv* env, jobject jo, jstring jkey) {
  const char* key = env->GetStringUTFChars(jkey, NULL);

  JSONObject* obj = (JSONObject*) env->GetLongField(jo, g_meobject_ptr);
  const char* value = obj->stringValue(key);

  env->ReleaseStringUTFChars(jkey, key);
  return env->NewStringUTF(value);
}

jdouble doubleValue(JNIEnv* env, jobject jo, jstring jkey) {
  const char* key = env->GetStringUTFChars(jkey, NULL);

  JSONObject* obj = (JSONObject*) env->GetLongField(jo, g_meobject_ptr);
  const double value = obj->doubleValue(key);
  env->ReleaseStringUTFChars(jkey, key);
  return (jdouble) value;
}

jint intValue(JNIEnv* env, jobject jo, jstring jkey) {
  const char* key = env->GetStringUTFChars(jkey, NULL);

  JSONObject* obj = (JSONObject*) env->GetLongField(jo, g_meobject_ptr);
  const int value = obj->intValue(key);
  env->ReleaseStringUTFChars(jkey, key);
  return (jint) value;
}

jlong longValue(JNIEnv* env, jobject jo, jstring jkey) {
  const char* key = env->GetStringUTFChars(jkey, NULL);

  JSONObject* obj = (JSONObject*) env->GetLongField(jo, g_meobject_ptr);
  const long value = obj->longValue(key);
  env->ReleaseStringUTFChars(jkey, key);
  return (jlong) value;
}

jfloat floatValue(JNIEnv* env, jobject jo, jstring jkey) {
  const char* key = env->GetStringUTFChars(jkey, NULL);

  JSONObject* obj = (JSONObject*) env->GetLongField(jo, g_meobject_ptr);
  const float value = obj->floatValue(key);
  env->ReleaseStringUTFChars(jkey, key);
  return (jfloat) value;
}

jboolean booleanValue(JNIEnv* env, jobject jo, jstring jkey) {
  const char* key = env->GetStringUTFChars(jkey, NULL);

  JSONObject* obj = (JSONObject*) env->GetLongField(jo, g_meobject_ptr);
  const bool_t value = obj->boolValue(key);
  env->ReleaseStringUTFChars(jkey, key);
  return (jboolean)(value == BS_TRUE);
}

jlong jsonValue(JNIEnv* env, jobject jo, jstring jkey)
{
  const char* key = env->GetStringUTFChars(jkey, NULL);

  JSONObject* obj = (JSONObject*) env->GetLongField(jo, g_meobject_ptr);
  const JSONObject value = obj->jsonValue(key);
  env->ReleaseStringUTFChars(jkey, key);
  return (jlong) obj;
}

jlong arrayValue(JNIEnv* env, jobject jo, jstring jkey)
{
    const char* key = env->GetStringUTFChars(jkey, NULL);

    JSONObject* obj = (JSONObject*) env->GetLongField(jo, g_meobject_ptr);
    const JSONArray value = obj->arrayValue(key);
    env->ReleaseStringUTFChars(jkey, key);
    return (jlong) obj;
}

