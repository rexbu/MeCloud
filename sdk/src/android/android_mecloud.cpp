/**
 * file :	android_mecloud.cpp
 * author :	Rex
 * create :	2017-07-17 18:50
 * func : 
 * history:
 */

#include <jni.h>
#include "MeAndroidCallback.h"
#include "GetAuthInfomationCallback.h"
#include "MeAndroidHttpFileCallBack.h"
#include <MeQuery.h>
#include <MeRole.h>
#include <MeDownloadFile.h>
#include <MeUploadFile.h>
#include <me/MeJoinQuery.h>
#include "MeACL.h"
#include "./mc/basic/McDevice.h"
#include "McDevice.h"

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif
void setJSONObjectPtr(JNIEnv *env, jobject thiz);
void setObjectPtr(JNIEnv *env, jobject thiz, jstring jfieldname, jstring jsign);
jstring char2Jstring(JNIEnv *env, const char *pat);
void global_exception(JNIEnv *env);
const char *getJclassName(JNIEnv *env, jclass objectClass);
void global_mecloud_callback(JNIEnv *env);

// JSONObject
jlong createJSONObject(JNIEnv *env, jobject thiz);
jlong createJSONObjectWithText(JNIEnv *env, jobject thiz, jstring text);
void destroy(JNIEnv *env, jobject jo);
jboolean has(JNIEnv *env, jobject thiz, jstring jname);
jstring jsonString(JNIEnv *env, jobject thiz);
jstring stringValue(JNIEnv *env, jobject jo, jstring jkey);
jdouble doubleValue(JNIEnv *env, jobject jo, jstring jkey);
jint intValue(JNIEnv *env, jobject jo, jstring jkey);
jlong longValue(JNIEnv *env, jobject jo, jstring jkey);
jfloat floatValue(JNIEnv *env, jobject jo, jstring jkey);
jboolean booleanValue(JNIEnv *env, jobject jo, jstring jkey);
jlong jsonValue(JNIEnv *env, jobject jo, jstring jkey);
jlongArray arrayValue(JNIEnv *env, jobject jo, jstring jkey);
jintArray intArrayValue(JNIEnv *env, jobject jo, jstring jkey);
jobjectArray stringArrayValue(JNIEnv *env, jobject jo, jstring jkey);

void jputString(JNIEnv *env, jobject jo, jstring jkey, jstring jvalue);
void jputDouble(JNIEnv *env, jobject jo, jstring jkey, jdouble jvalue);
void jputInt(JNIEnv *env, jobject jo, jstring jkey, jint jvalue);
void jpMeUploadFileutLong(JNIEnv *env, jobject jo, jstring jkey, jlong jvalue);
void jputLong(JNIEnv *env, jobject jo, jstring jkey, jlong jvalue);
void jputFloat(JNIEnv *env, jobject jo, jstring jkey, jfloat jvalue);
void jputBoolean(JNIEnv *env, jobject jo, jstring jkey, jboolean jvalue);
void jputObject(JNIEnv *env, jobject jo, jstring jkey, jlong meObjectObjectPre);

// MeObject
jlong createMeObject(JNIEnv *env, jobject thiz);
jlong createMeObjectWithClassName(JNIEnv *env, jobject thiz, jstring className);
jlong createMeObjectWithJSONObject(JNIEnv *env, jobject thiz, jstring jname, jlong jobjectPre);
jlong createMeObjectWithObjectId(JNIEnv *env, jobject thiz, jstring jobjectid, jstring jclassName);
jstring objectId(JNIEnv *env, jobject thiz);
jstring className(JNIEnv *env, jobject thiz);
void setClassName(JNIEnv *env, jobject thiz, jstring className);
void setACL(JNIEnv *env, jobject thiz, jlong meACLObjectPre);
jlong getACL(JNIEnv *env, jobject thiz, jlong meACLObjectPre);
void putObject(JNIEnv *env, jobject jo, jstring jkey, jlong meObjectObjectPre);
jlongArray objectValue(JNIEnv *env, jobject jo, jstring jkey);
// MeQuery
jlong createMeQuery(JNIEnv *env, jobject thiz);
jlong createMeQueryWithClassName(JNIEnv *env, jobject thiz, jstring jclassName);
void whereEqualToString(JNIEnv *env, jobject thiz, jstring jkey, jstring jval);
void whereNotEqualToString(JNIEnv *env, jobject thiz, jstring jkey,
                           jstring jval);
void whereEqualToInt(JNIEnv *env, jobject thiz, jstring jkey, jint jval);
void whereNotEqualToInt(JNIEnv *env, jobject thiz, jstring jkey, jint jval);
void whereEqualOr(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue);
void whereEqualOrToInt(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue);
void whereGreaterToString(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue);
void whereGreaterToInt(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue);
void whereLessToString(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue);
void whereLessToInt(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue);

void selectKeys(JNIEnv *env, jobject thiz, jobjectArray jkeys, jint num);
void addSelectKey(JNIEnv *env, jobject thiz, jstring jkey);
void addNotSelectKey(JNIEnv *env, jobject thiz, jstring jkey);
void addAscendSortKeys(JNIEnv *env, jobject thiz, jstring jkey);
void addDescendSortKeys(JNIEnv *env, jobject thiz, jstring jkey);
void addLimit(JNIEnv *env, jobject thiz, jlong jcount);
void addOffset(JNIEnv *env, jobject thiz, jstring startId);
void addAggregateObject(JNIEnv *env, jobject thiz, jobject jaggreate);
void setAggregateObject(JNIEnv *env, jobject thiz, jobject jaggreate);

// MeRole
jlong createMeRole(JNIEnv *env, jobject thiz, jstring jrolename);
void setUserId(JNIEnv *env, jobject thiz, jstring juserid);
void setUserObjects(JNIEnv *env, jobject thiz, jlong jmeUser);

// MeCloud
void initialize(JNIEnv *env, jobject thiz, jstring jappId, jstring jappKey);
void initialize(JNIEnv *env, jobject thiz, jstring jappId, jstring jappKey);
void config(JNIEnv *env, jobject thiz);
void setBaseUrl(JNIEnv *env, jobject thiz, jstring jurl);
void setTimeout(JNIEnv *env, jobject thiz, jint time);
void setShowLog(JNIEnv *env, jobject thiz, jboolean showLog);
void addHttpHeader(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue);
jbyteArray crypto(JNIEnv *env, jobject thiz, jbyteArray byteArray);
jbyteArray decrypt(JNIEnv *env, jobject thiz, jbyteArray byteArray);
jstring cookie(JNIEnv *env, jobject thiz);
void login(JNIEnv *env, jobject thiz, jstring jusername, jstring jpassword, jobject callback);
void signUp(JNIEnv *env, jobject thiz, jstring jusername, jstring jpassword, jobject callback);
void changePassword(JNIEnv *env, jobject thiz, jstring jusername, jstring jpassword, jobject callback);
void save(JNIEnv *env, jobject thiz, jobject object, jobject jcallback);
void getObjectWithID(JNIEnv *env, jobject thiz, jstring jobjectId, jstring jclassName, jobject jcallback);
void getObjectsWithQuery(JNIEnv *env, jobject thiz, jobject object, jobject jcallback);
void findWithJoin(JNIEnv *env, jobject thiz, jobject object, jobject callBack);
void queryWithUrl(JNIEnv *env, jobject thiz, jstring jurl, jlong jJsonObject, jobject jcallback);
void deleteObject(JNIEnv *env, jobject thiz, jlong jJsonObject, jobject jcallback);
jstring download(JNIEnv *env, jobject thiz, jstring name, jstring jurl, jobject jcallback);
void uploadFile(JNIEnv *env, jobject thiz, jstring type, jstring jpath, jobject jcallback);
void uploadData(JNIEnv *env, jobject thiz, jstring type, jbyteArray jdata, jobject jcallback);
void postData(JNIEnv *env, jobject thiz, jstring jurl, jbyteArray jData, jobject jcallBack);
void saveWithUrl(JNIEnv *env, jobject thiz, jstring jurl, jobject jJsonObject, jobject jcallback);
void getWithUrl(JNIEnv *env, jobject thiz, jstring jurl, jobject jJsonObject, jobject jcallback);

// MeFile
jlong createMeFile(JNIEnv *env, jobject thiz, jstring jclassName);
jlong createMeFileWithObjectId(JNIEnv *env, jobject thiz, jstring jobjId, jstring jclassName);
jstring filePath(JNIEnv *env, jobject thiz);

jstring imageUrl(JNIEnv *env, jobject thiz, jint width, jint height);
jstring imageUrlNormal(JNIEnv *env, jobject thiz);
jstring imageCropUrl(JNIEnv *env, jobject thiz, jint x, jint y, jint width, jint height);

// MeUser
jlong createUser(JNIEnv *env, jobject thiz);
jlong createUserWithJSONObject(JNIEnv *env, jobject thiz, jlong jsonPtr);
jlong currentUser(JNIEnv *env, jobject thiz);
void logout(JNIEnv *env, jobject thiz);
void saveLoginUser(JNIEnv *env, jobject thiz, jlong userPtr);
jstring encodePassword(JNIEnv *env, jobject thiz, jstring jusername, jstring jpassword);
jstring device(JNIEnv *env, jobject thiz);

// MeACL
void setPublicReadAccess(JNIEnv *env, jobject thiz);
void setPublicWriteAccess(JNIEnv *env, jobject thiz);
void setRoleReadAccess(JNIEnv *env, jobject thiz, jstring jrole);
void setRoleWriteAccess(JNIEnv *env, jobject thiz, jstring jrole);
void setRoleReadAccessMeRole(JNIEnv *env, jobject thiz, jlong objectPre);
void setRoleWriteAccessMeRole(JNIEnv *env, jobject thiz, jlong objectPre);
void setUserReadAccess(JNIEnv *env, jobject thiz, jstring juserId);
void setUserWriteAccess(JNIEnv *env, jobject thiz, jstring juserId);
void setUserReadAccessMeUser(JNIEnv *env, jobject thiz, jlong objectPre);
void setUserWriteAccessMeUser(JNIEnv *env, jobject thiz, jlong objectPre);

// MeJoinQuery
jlong createMeJoinQuery(JNIEnv *env, jobject thiz, jstring jclassName, jboolean jnestQuery);
void addSelectKeyWithJoin(JNIEnv *env, jobject thiz, jstring key);
void addNotSelectKeyWithJoin(JNIEnv *env, jobject thiz, jstring key);
void addAscendWithJoin(JNIEnv *env, jobject thiz, jstring key);
void addDescendWithJoin(JNIEnv *env, jobject thiz, jstring key);

void matchEqualTo(JNIEnv *env, jobject thiz, jstring key, jstring value);
void matchEqualToInt(JNIEnv *env, jobject thiz, jstring jkey, jint value);
void matchGreater(JNIEnv *env, jobject thiz, jstring jkey, jstring value);
void matchGreaterToInt(JNIEnv *env, jobject thiz, jstring jkey, jint value);
void matchLess(JNIEnv *env, jobject thiz, jstring key, jstring value);
void matchLessToInt(JNIEnv *env, jobject thiz, jstring jkey, jint value);
void addForeignTable(JNIEnv *env, jobject thiz, jstring fromTable, jstring foreignKey, jstring localKey, jstring jdocument);
void addMeJoinQueryPtr(JNIEnv *env, jobject thiz, jlong joinQuery);
void addLimitWithJoin(JNIEnv *env, jobject thiz, jint count);

// MeAggregateObject
jlong createMeAggregateObject(JNIEnv *env, jobject thiz, jstring jclassName);
void whereEqualToWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue);
void whereEqualToIntWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue);
void whereNotEqualToWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue);
void whereNotEqualToIntWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue);
void whereGreaterWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue);
void whereGreaterIntWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue);
void whereLessWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue);
void whereLessIntWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue);
void setResponseKey(JNIEnv *env, jobject thiz, jstring jkey);
void setDistinctKey(JNIEnv *env, jobject thiz, jstring jkey);
void setMethod(JNIEnv *env, jobject thiz, jint method);

void setDevice(JNIEnv *env, jobject thiz, jobject jcontext);
void saveJSONToCache(JNIEnv *env, jobject thiz, jlong objectPre, jstring jkey);
jlong getJSONFromCache(JNIEnv *env, jobject thiz, jstring jkey);
void removeJSONFromCache(JNIEnv *env, jobject thiz, jstring jkey);

#ifdef __cplusplus
}
#endif

//jclass g_class = NULL;
jfieldID g_ptr = NULL;

jmethodID g_mequery_getCallback = NULL;
jmethodID g_mequery_findCallback = NULL;

jclass g_exception_class = NULL;
jmethodID g_exception_init = NULL;

jmethodID g_mecloud_callback = NULL;
jmethodID g_mecloud_callbackList = NULL;

const char *getJclassName(JNIEnv *env, jclass objectClass) {
    if (objectClass) {
        jclass classClass = env->FindClass("java/lang/Class");
        jmethodID getClassNameMethodId = env->GetMethodID(classClass,
                                                          "getSimpleName",
                                                          "()Ljava/lang/String;");
        jstring javaClassNameJString = (jstring) env->CallObjectMethod(
                objectClass, getClassNameMethodId, "");
        const char *tempClassName = env->GetStringUTFChars(javaClassNameJString,
                                                           NULL);
        char *className = new char[strlen(tempClassName) + 1];
        strcpy(className, tempClassName);
        env->ReleaseStringUTFChars(javaClassNameJString, tempClassName);
        env->DeleteLocalRef(classClass);
        env->DeleteLocalRef(javaClassNameJString);
        return className;
    } else {
        return NULL;
    }
}

void global_exception(JNIEnv *env) {
    if (g_exception_class == NULL) {
        g_exception_class = (jclass) env->NewGlobalRef(
                env->FindClass("com/rex/mecloud/MeException"));
        g_exception_init = env->GetMethodID(
                g_exception_class, "<init>",
                "(ILjava/lang/String;Ljava/lang/String;)V");
    }
}

void global_mecloud_callback(JNIEnv *env) {
    if (g_mecloud_callback == NULL || g_mecloud_callbackList == NULL) {
        jclass mecloud_class = env->FindClass("com/rex/mecloud/MeCloud");
        g_mecloud_callback = env->GetMethodID(mecloud_class,
                                              "callback",
                                              "(JLcom/rex/mecloud/MeCallback;Lcom/rex/mecloud/MeException;)V");
        g_mecloud_callbackList = env->GetMethodID(mecloud_class,
                                                  "callbackList",
                                                  "([JLcom/rex/mecloud/MeListCallback;Lcom/rex/mecloud/MeException;)V");
        env->DeleteLocalRef(mecloud_class);
    }
}

void setObjectPtr(JNIEnv *env, jobject thiz, jstring jfieldname,
                  jstring jsign) {
    const char *fieldName = env->GetStringUTFChars(jfieldname, NULL);
    const char *sign = env->GetStringUTFChars(jsign, NULL);
    jclass clazz = env->FindClass("com/rex/mecloud/JSONObject");
    g_ptr = env->GetFieldID(clazz, fieldName, sign);
    global_exception(env);
    env->DeleteLocalRef(clazz);
    env->ReleaseStringUTFChars(jfieldname, fieldName);
    env->ReleaseStringUTFChars(jsign, sign);
}

void setJSONObjectPtr(JNIEnv *env, jobject thiz) {
    jstring name = env->NewStringUTF("objectPtr");
    jstring sign = env->NewStringUTF("J");
    setObjectPtr(env, thiz, name, sign);
    env->DeleteLocalRef(name);
    env->DeleteLocalRef(sign);
}

//MeObject
jlong createMeObject(JNIEnv *env, jobject thiz) {
    setJSONObjectPtr(env, thiz);
    MeObject *meObject = new MeObject();
    return (jlong) meObject;
}

jlong createMeObjectWithClassName(JNIEnv *env, jobject thiz, jstring jclassName) {
    setJSONObjectPtr(env, thiz);
    const char *className = env->GetStringUTFChars(jclassName, NULL);
    MeObject *obj = new MeObject(className);
    env->ReleaseStringUTFChars(jclassName, className);
    return (jlong) obj;
}

jlong createMeObjectWithJSONObject(JNIEnv *env, jobject thiz, jstring jclassName, jlong jobjectPre) {
    setJSONObjectPtr(env, thiz);
    const char *className = env->GetStringUTFChars(jclassName, NULL);
    JSONObject *jsonObject = (JSONObject *) jobjectPre;
    if (jobjectPre) {
        MeObject *obj = new MeObject(className, jsonObject);
        env->ReleaseStringUTFChars(jclassName, className);
        return (jlong) obj;
    } else {
        env->ReleaseStringUTFChars(jclassName, className);
        return -1;
    }
}

jlong createMeObjectWithObjectId(JNIEnv *env, jobject thiz, jstring jobjectid, jstring jclassName) {
    setJSONObjectPtr(env, thiz);
    const char *objectid = env->GetStringUTFChars(jobjectid, NULL);
    const char *classname = env->GetStringUTFChars(jclassName, NULL);
    MeObject *meObject = new MeObject(objectid, classname);
    env->ReleaseStringUTFChars(jobjectid, objectid);
    env->ReleaseStringUTFChars(jclassName, classname);
    return (jlong) meObject;
}


void destroy(JNIEnv *env, jobject thiz) {
    if (thiz != NULL && g_ptr != NULL) {
        JSONObject *obj = (JSONObject *) env->GetLongField(thiz, g_ptr);
        if (obj != NULL) {
            delete obj;
            obj = NULL;
        }
    }
}

jstring objectId(JNIEnv *env, jobject thiz) {
    MeObject *obj = (MeObject *) env->GetLongField(thiz, g_ptr);
    return env->NewStringUTF(obj->objectId());
}

jstring className(JNIEnv *env, jobject thiz) {
    MeObject *obj = (MeObject *) env->GetLongField(thiz, g_ptr);
    return env->NewStringUTF(obj->className());
}

void setClassName(JNIEnv *env, jobject thiz, jstring className) {
    MeObject *obj = (MeObject *) env->GetLongField(thiz, g_ptr);
    const char *name = env->GetStringUTFChars(className, NULL);
    obj->setClassName(name);
    env->ReleaseStringUTFChars(className, name);
}

void setACL(JNIEnv *env, jobject thiz, jlong meACLObjectPre) {
    MeACL *meACL = (MeACL *) meACLObjectPre;
    if (meACL) {
        MeObject *obj = (MeObject *) env->GetLongField(thiz, g_ptr);
        obj->setACL(meACL);
    }
}

jlong getACL(JNIEnv *env, jobject thiz, jlong meACLObjectPre) {
    MeObject *obj = (MeObject *) env->GetLongField(thiz, g_ptr);
    MeACL *meACL = new MeACL(obj->getACL());
    return (jlong) meACL;
}

void putObject(JNIEnv *env, jobject jo, jstring jkey, jlong meObjectObjectPre) {
    JSONObject *meObject = (JSONObject *) meObjectObjectPre;
    if (meObject != NULL) {
        const char *key = env->GetStringUTFChars(jkey, NULL);
        JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
        obj->put(key, meObject);
        env->ReleaseStringUTFChars(jkey, key);
    }
}

jlongArray objectValue(JNIEnv *env, jobject jo, jstring jkey) {
    if (jkey == NULL) {
        return NULL;
    }
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeObject *obj = (MeObject *) env->GetLongField(jo, g_ptr);
    JSONArray array = obj->arrayValue(key);
    //NSMutableArray *jsonArray = [NSMutableArray array];
    //创建一个 MeObject 数组
    int size = array.size();
    jlongArray value = env->NewLongArray(size);
    for (int i = 0; i < size; ++i) {
        JSONObject object = array.jsonValue(i);
        MeObject *childObject = new MeObject();
        childObject->copy(&object, false);
        env->SetLongArrayRegion(value, i, 1, (const jlong *) &childObject);
        delete childObject;
    }

    env->ReleaseStringUTFChars(jkey, key);
    return value;//返回数组指针
}

//JSONObject
jlong createJSONObject(JNIEnv *env, jobject thiz) {
    setJSONObjectPtr(env, thiz);
    return (jlong) new JSONObject();
}

jlong createJSONObjectWithText(JNIEnv *env, jobject thiz, jstring text) {
    setJSONObjectPtr(env, thiz);
    const char *content = env->GetStringUTFChars(text, NULL);
    JSONObject *newObject = new JSONObject(content);
    return (jlong) newObject;
}

jboolean has(JNIEnv *env, jobject thiz, jstring jname) {
    const char *name = env->GetStringUTFChars(jname, NULL);
    JSONObject *obj = (JSONObject *) env->GetLongField(thiz, g_ptr);
    env->ReleaseStringUTFChars(jname, name);
    return (jboolean) (obj->has(name));
}

jstring jsonString(JNIEnv *env, jobject thiz) {
    JSONObject *obj = (JSONObject *) env->GetLongField(thiz, g_ptr);
    const char *string = obj->toString();
    jstring newString = env->NewStringUTF(string);
    return newString;
}

jstring stringValue(JNIEnv *env, jobject jo, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    const char *value = obj->stringValue(key);
    env->ReleaseStringUTFChars(jkey, key);
    return env->NewStringUTF(value);
}

jdouble doubleValue(JNIEnv *env, jobject jo, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    const double value = obj->doubleValue(key);
    env->ReleaseStringUTFChars(jkey, key);
    return (jdouble) value;
}

jint intValue(JNIEnv *env, jobject jo, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    const int value = obj->intValue(key);
    env->ReleaseStringUTFChars(jkey, key);
    return (jint) value;
}

jlong longValue(JNIEnv *env, jobject jo, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    const long value = obj->longValue(key);
    env->ReleaseStringUTFChars(jkey, key);
    return (jlong) value;
}

jfloat floatValue(JNIEnv *env, jobject jo, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    const float value = obj->floatValue(key);
    env->ReleaseStringUTFChars(jkey, key);
    return (jfloat) value;
}

jboolean booleanValue(JNIEnv *env, jobject jo, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    const bool_t value = obj->boolValue(key);
    env->ReleaseStringUTFChars(jkey, key);
    return (jboolean) (value == BS_TRUE);
}

jlong jsonValue(JNIEnv *env, jobject jo, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    JSONObject value = obj->jsonValue(key);
    env->ReleaseStringUTFChars(jkey, key);
    JSONObject *copyValue = new JSONObject(&value, false);
    return (jlong) copyValue;
}

jlongArray arrayValue(JNIEnv *env, jobject jo, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    JSONArray value = obj->arrayValue(key);
    int size = value.size();
    jlongArray array = env->NewLongArray(size);
    jlong longArray[size];
    for (int i = 0; i < size; ++i) {
        JSONObject object = value.jsonValue(i);
        JSONObject *objectPtr = new JSONObject(&object, false);
        longArray[i] = (jlong)objectPtr;
    }
    env->SetLongArrayRegion(array, 0, size, longArray);
    env->ReleaseStringUTFChars(jkey, key);
    return array;
}

jintArray intArrayValue(JNIEnv *env, jobject jo, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    JSONArray jsonArray = obj->arrayValue(key);
    int size = jsonArray.size();
    jintArray array = env->NewIntArray(size);
    jint intArray[size];
    for (int i = 0; i < size; ++i) {
        int *value = jsonArray.intValue(i);
        intArray[i]= (jint) *value;
    }
    env->SetIntArrayRegion(array, 0, size, intArray);
    env->ReleaseStringUTFChars(jkey, key);
    return array;
}

jobjectArray stringArrayValue(JNIEnv *env, jobject jo, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    JSONArray jsonArray = obj->arrayValue(key);
    int size = jsonArray.size();
    jobjectArray args = (env)->NewObjectArray(size, (env)->FindClass("java/lang/String"), 0);
    for (int i = 0; i < size; ++i) {
        jstring str = (env)->NewStringUTF(jsonArray.stringValue(i));
        (env)->SetObjectArrayElement(args, i, str);
    }
    env->ReleaseStringUTFChars(jkey, key);
    return args;
}

void jputString(JNIEnv *env, jobject jo, jstring jkey, jstring jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *value = env->GetStringUTFChars(jvalue, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    obj->put(key, value);

    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jvalue, value);
}

void jputDouble(JNIEnv *env, jobject jo, jstring jkey, jdouble jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    obj->put(key, jvalue);

    env->ReleaseStringUTFChars(jkey, key);
}

void jputInt(JNIEnv *env, jobject jo, jstring jkey, jint jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    obj->put(key, jvalue);

    env->ReleaseStringUTFChars(jkey, key);
}

void jputLong(JNIEnv *env, jobject jo, jstring jkey, jlong jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    obj->put(key, (long) jvalue);
    env->ReleaseStringUTFChars(jkey, key);
}

void jputFloat(JNIEnv *env, jobject jo, jstring jkey, jfloat jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);

    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    obj->put(key, jvalue);

    env->ReleaseStringUTFChars(jkey, key);
}

void jputBoolean(JNIEnv *env, jobject jo, jstring jkey, jboolean jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const bool value = jvalue;
    JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
    obj->put(key, value);

    env->ReleaseStringUTFChars(jkey, key);
}

void jputObject(JNIEnv *env, jobject jo, jstring jkey,
                jlong meObjectObjectPre) {
    MeObject *meObject = (MeObject *) meObjectObjectPre;
    if (meObject != NULL) {
        const char *key = env->GetStringUTFChars(jkey, NULL);
        JSONObject *obj = (JSONObject *) env->GetLongField(jo, g_ptr);
        obj->put(key, meObject);
        env->ReleaseStringUTFChars(jkey, key);
    }
}

// MeQury
jlong createMeQuery(JNIEnv *env, jobject thiz) {
    setJSONObjectPtr(env, thiz);
    MeQuery *obj = new MeQuery();
    return (jlong) obj;
}

jlong createMeQueryWithClassName(JNIEnv *env, jobject thiz, jstring jclassName) {
    setJSONObjectPtr(env, thiz);
    const char *className = env->GetStringUTFChars(jclassName, NULL);
    MeQuery *obj = new MeQuery(className);
    env->ReleaseStringUTFChars(jclassName, className);
    return (jlong) obj;
}

void whereEqualToString(JNIEnv *env, jobject thiz, jstring jkey, jstring jval) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *val = env->GetStringUTFChars(jval, NULL);
    MeQuery *obj = (MeQuery *) env->GetLongField(thiz, g_ptr);
    obj->whereEqualTo(key, val);
    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jval, val);
}

void whereNotEqualToString(JNIEnv *env, jobject thiz, jstring jkey,
                           jstring jval) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *val = env->GetStringUTFChars(jval, NULL);
    MeQuery *obj = (MeQuery *) env->GetLongField(thiz, g_ptr);
    obj->whereNotEqualTo(key, val);
    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jval, val);
}

void whereEqualToInt(JNIEnv *env, jobject thiz, jstring jkey, jint val) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeQuery *obj = (MeQuery *) env->GetLongField(thiz, g_ptr);
    obj->whereEqualTo(key, val);
    env->ReleaseStringUTFChars(jkey, key);
}

void whereNotEqualToInt(JNIEnv *env, jobject thiz, jstring jkey, jint val) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeQuery *obj = (MeQuery *) env->GetLongField(thiz, g_ptr);
    obj->whereNotEqualTo(key, val);
    env->ReleaseStringUTFChars(jkey, key);
}

void whereEqualOr(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *value = env->GetStringUTFChars(jvalue, NULL);
    MeQuery *query = (MeQuery *) env->GetLongField(thiz, g_ptr);
    query->whereEqualOr(key, value);
    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jvalue, value);
}

void whereEqualOrToInt(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeQuery *query = (MeQuery *) env->GetLongField(thiz, g_ptr);
    query->whereEqualOr(key, jvalue);
    env->ReleaseStringUTFChars(jkey, key);
}

void whereGreaterToString(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *value = env->GetStringUTFChars(jvalue, NULL);
    MeQuery *query = (MeQuery *) env->GetLongField(thiz, g_ptr);
    query->whereGreater(key, value);
    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jvalue, value);
}

void whereGreaterToInt(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeQuery *query = (MeQuery *) env->GetLongField(thiz, g_ptr);
    query->whereGreater(key, jvalue);
    env->ReleaseStringUTFChars(jkey, key);
}

void whereLessToString(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *value = env->GetStringUTFChars(jvalue, NULL);
    MeQuery *query = (MeQuery *) env->GetLongField(thiz, g_ptr);
    query->whereLess(key, value);
    env->ReleaseStringUTFChars(jkey, key);
}

void whereLessToInt(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeQuery *query = (MeQuery *) env->GetLongField(thiz, g_ptr);
    query->whereLess(key, jvalue);
    env->ReleaseStringUTFChars(jkey, key);
}

void selectKeys(JNIEnv *env, jobject thiz, jobjectArray jkeys, jint num) {
    const jsize size = env->GetArrayLength((jarray) jkeys);
    const char **keys = (const char **) new char *[size];
    for (int i = 0; i < size; ++i) {
        jstring tempString = (jstring) env->GetObjectArrayElement(jkeys, size);
        const char *key = env->GetStringUTFChars(tempString, NULL);
        keys[size] = key;
        env->DeleteLocalRef(tempString);
    }
    MeQuery *obj = (MeQuery *) env->GetLongField(thiz, g_ptr);
    obj->selectKeys(keys, num);
}

void addSelectKey(JNIEnv *env, jobject thiz, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeQuery *obj = (MeQuery *) env->GetLongField(thiz, g_ptr);
    obj->addSelectKey(key);
    env->ReleaseStringUTFChars(jkey, key);
}

void addNotSelectKey(JNIEnv *env, jobject thiz, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeQuery *obj = (MeQuery *) env->GetLongField(thiz, g_ptr);
    obj->addNotSelectKey(key);
    env->ReleaseStringUTFChars(jkey, key);
}

void addAscendSortKeys(JNIEnv *env, jobject thiz, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeQuery *obj = (MeQuery *) env->GetLongField(thiz, g_ptr);
    obj->addAscendSortKeys(key);
    env->ReleaseStringUTFChars(jkey, key);
}

void addDescendSortKeys(JNIEnv *env, jobject thiz, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeQuery *obj = (MeQuery *) env->GetLongField(thiz, g_ptr);
    obj->addDescendSortKeys(key);
    env->ReleaseStringUTFChars(jkey, key);
}

void addLimit(JNIEnv *env, jobject thiz, jlong jcount) {
    MeQuery *obj = (MeQuery *) env->GetLongField(thiz, g_ptr);
    obj->addLimit(jcount);
}

void addOffset(JNIEnv *env, jobject thiz, jstring startId) {
    MeQuery *obj = (MeQuery *) env->GetLongField(thiz, g_ptr);
    const char *id = env->GetStringUTFChars(startId, NULL);
    obj->addStartId(id);
    env->ReleaseStringUTFChars(startId, id);
}

void addAggregateObject(JNIEnv *env, jobject thiz, jobject jaggregate) {
    MeQuery *query = (MeQuery *) env->GetLongField(thiz, g_ptr);
    MeAggregateObject *aggregate = (MeAggregateObject *)env->GetLongField(jaggregate, g_ptr);
    query->addAggregateObject(aggregate);
}

void setAggregateObject(JNIEnv *env, jobject thiz, jobject jaggregate) {
    MeQuery *obj = (MeQuery *) env->GetLongField(thiz, g_ptr);
    MeAggregateObject *aggregate = (MeAggregateObject *) env->GetLongField(jaggregate, g_ptr);
    obj->setAggregateObject(aggregate);
}


//MeRole
jlong createMeRole(JNIEnv *env, jobject thiz, jstring jrolename) {
    setJSONObjectPtr(env, thiz);
    const char *roleName = env->GetStringUTFChars(jrolename, NULL);
    MeRole *obj = new MeRole(roleName);
    env->ReleaseStringUTFChars(jrolename, roleName);
    return (jlong) obj;
}

void setUserId(JNIEnv *env, jobject thiz, jstring juserid) {
    const char *userid = env->GetStringUTFChars(juserid, NULL);
    MeRole *obj = (MeRole *) env->GetLongField(thiz, g_ptr);
    obj->setUser(userid);
    env->ReleaseStringUTFChars(juserid, userid);
}

void setUserObjects(JNIEnv *env, jobject thiz, jlong jmeUser) {
    MeUser *meUser = (MeUser *) jmeUser;
    if (meUser != NULL) {
        MeRole *obj = (MeRole *) env->GetLongField(thiz, g_ptr);
        obj->setUser(meUser);
    }
}

// MeFile
jlong createMeFile(JNIEnv *env, jobject thiz, jstring jclassName) {
    setJSONObjectPtr(env, thiz);
    const char *className = env->GetStringUTFChars(jclassName, NULL);
    MeFile *obj = new MeFile(ME_HTTPFILE_Default, className);
    env->ReleaseStringUTFChars(jclassName, className);
    return (jlong)obj;
}

jlong createMeFileWithObjectId(JNIEnv *env, jobject thiz, jstring jobjId, jstring jclassName) {
    setJSONObjectPtr(env, thiz);
    const char* objId = env->GetStringUTFChars(jobjId, NULL);
    const char* classname = env->GetStringUTFChars(jclassName, NULL);
    MeFile *file = new MeFile(objId, classname);
    env->ReleaseStringUTFChars(jobjId, objId);
    env->ReleaseStringUTFChars(jclassName, classname);
    return (jlong) file;
}

jstring filePath(JNIEnv *env, jobject thiz) {
    MeFile *obj = (MeFile *) env->GetLongField(thiz, g_ptr);
    return env->NewStringUTF(obj->filePath());
}

jstring imageUrl(JNIEnv *env, jobject thiz, jint width, jint height) {
    MeFile *file = (MeFile *) env->GetLongField(thiz, g_ptr);
    const char *url = file->imageUrl(width, height);
    return env->NewStringUTF(url);

}
jstring imageUrlNormal(JNIEnv *env, jobject thiz) {
    MeFile *file = (MeFile *) env->GetLongField(thiz, g_ptr);
    const char *url = file->imageUrl();
    return env->NewStringUTF(url);
}

jstring imageCropUrl(JNIEnv *env, jobject thiz, jint x, jint y, jint width, jint height) {
    MeFile *file = (MeFile *) env->GetLongField(thiz, g_ptr);
    const char *url = file->imageCropUrl(x, y, width, height);
    return env->NewStringUTF(url);
}

// MeCloud
void initialize(JNIEnv *env, jobject thiz, jstring jappId, jstring jappKey) {
    const char *appId = env->GetStringUTFChars(jappId, NULL);
    const char *appKey = env->GetStringUTFChars(jappKey, NULL);
    MeCloud::initialize(appId, appKey);
    env->ReleaseStringUTFChars(jappId, appId);
    env->ReleaseStringUTFChars(jappKey, appKey);
    config(env, thiz);
}

void config(JNIEnv *env, jobject thiz) {
    global_mecloud_callback(env);
}

jstring cookie(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF(MeCloud::shareInstance()->cookie());
}

void setBaseUrl(JNIEnv *env, jobject thiz, jstring jurl) {
    MeCloud::shareInstance()->setBaseUrl(env->GetStringUTFChars(jurl, NULL));
}

void setTimeout(JNIEnv *env, jobject thiz, jint time) {
    MeCloud::shareInstance()->setTimeout(time);
}

void setShowLog(JNIEnv *env, jobject thiz, jboolean showLog) {
    MeCloud::showLog(showLog);
}

jbyteArray crypto(JNIEnv *env, jobject thiz, jbyteArray byteArray) {
    jsize size = env->GetArrayLength(byteArray);
    unsigned char *data = (unsigned char *) env->GetPrimitiveArrayCritical(byteArray, NULL);
    unsigned char *cryptoData = MeCloud::shareInstance()->crypto(data, size);
    env->ReleasePrimitiveArrayCritical(byteArray, data, NULL);
    jbyteArray jarray = env->NewByteArray(size);
    env->SetByteArrayRegion(jarray, 0, size, (jbyte *) cryptoData);
    return jarray;
}

jbyteArray decrypt(JNIEnv *env, jobject thiz, jbyteArray byteArray) {
    jsize size = env->GetArrayLength(byteArray);
    unsigned char *data = (unsigned char *) env->GetPrimitiveArrayCritical(byteArray, NULL);
    unsigned char *decryptData = MeCloud::shareInstance()->decrypt(data, size);
    env->ReleasePrimitiveArrayCritical(byteArray, data, NULL);
    jbyteArray jarray = env->NewByteArray(size);//这行代码错误了
    env->SetByteArrayRegion(jarray, 0, size, (jbyte *) decryptData);
    return jarray;
}

void addHttpHeader(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *value = env->GetStringUTFChars(jvalue, NULL);
    MeCloud::shareInstance()->addHttpHeader(key, value);
    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jvalue, value);
}

void login(JNIEnv *env, jobject thiz, jstring jusername, jstring jpassword, jobject jcallback) {
    const char *username = env->GetStringUTFChars(jusername, NULL);
    const char *password = env->GetStringUTFChars(jpassword, NULL);
    jobject global_thiz = env->NewGlobalRef(thiz);
    jobject global_callback = env->NewGlobalRef(jcallback);
    MeUser *user = new MeUser();
    MeObjectCallback *callback = new MeUserCallback(user);
    callback->setPara(global_thiz, global_callback);
    callback->lock();
    user->login(username, password, callback);
    env->ReleaseStringUTFChars(jusername, username);
    env->ReleaseStringUTFChars(jpassword, password);
}

void signUp(JNIEnv *env, jobject thiz, jstring jusername, jstring jpassword, jobject jcallback) {
    const char *username = env->GetStringUTFChars(jusername, NULL);
    const char *password = env->GetStringUTFChars(jpassword, NULL);
    jobject global_thiz = env->NewGlobalRef(thiz);
    jobject global_callback = env->NewGlobalRef(jcallback);
    MeUser *user = new MeUser();
    MeUserCallback *callback = new MeUserCallback(user);
    callback->setPara(global_thiz, global_callback);
    callback->lock();
    user->signup(username, password, callback);
    env->ReleaseStringUTFChars(jusername, username);
    env->ReleaseStringUTFChars(jpassword, password);
}

void changePassword(JNIEnv *env, jobject thiz, jstring jusername, jstring jnewpassword,
                    jobject jcallback) {
    const char *username = env->GetStringUTFChars(jusername, NULL);
    const char *newpassword = env->GetStringUTFChars(jnewpassword, NULL);
    jobject global_thiz = env->NewGlobalRef(thiz);
    jobject global_callback = env->NewGlobalRef(jcallback);
    MeUser *user = new MeUser();
    MeObjectCallback *callback = new MeUserCallback(user);
    callback->setPara(global_thiz, global_callback);
    callback->lock();
    user->changePassword(username, newpassword, callback);
    env->ReleaseStringUTFChars(jusername, username);
    env->ReleaseStringUTFChars(jnewpassword, newpassword);
}

void save(JNIEnv *env, jobject thiz, jobject object, jobject jcallback) {
    //传入的object组要做全局引用，否则函数返回后会被释放
    jobject global_thiz = env->NewGlobalRef(thiz);
    jobject global_callback = env->NewGlobalRef(jcallback);
    MeObject *obj = (MeObject *) env->GetLongField(object, g_ptr);
    MeObjectCallback *callback = new MeObjectCallback(global_thiz, global_callback);
    callback->setIsMeListCallback(BS_FALSE);
    callback->lock();
    obj->save(callback);
}

void getObjectWithID(JNIEnv *env, jobject thiz, jstring jobjectId, jstring jclassName, jobject jcallback) {
    jobject global_thiz = env->NewGlobalRef(thiz);
    jobject global_callback = env->NewGlobalRef(jcallback);
    MeObjectCallback *callback = new MeObjectCallback(global_thiz, global_callback);
    callback->setIsMeListCallback(BS_FALSE);
    const char *objectId = env->GetStringUTFChars(jobjectId, NULL);
    const char *className = env->GetStringUTFChars(jclassName, NULL);
    MeQuery *query = new MeQuery(className);
    query->get(objectId, callback);
    env->ReleaseStringUTFChars(jobjectId, objectId);
    env->ReleaseStringUTFChars(jclassName, className);
}

void getObjectsWithQuery(JNIEnv *env, jobject thiz, jobject object, jobject jcallback) {
    jobject global_thiz = env->NewGlobalRef(thiz);
    jobject global_callback = env->NewGlobalRef(jcallback);
    MeObjectCallback *callback = new MeObjectCallback(global_thiz, global_callback);
    callback->setIsMeListCallback(BS_TRUE);
    MeQuery *obj = (MeQuery *) env->GetLongField(object, g_ptr);
    obj->find(callback);
}

void findWithJoin(JNIEnv *env, jobject thiz, jobject object, jobject jcallBack) {
    jobject global_thiz = env->NewGlobalRef(thiz);
    jobject global_callback = env->NewGlobalRef(jcallBack);
    MeObjectCallback *callback = new MeObjectCallback(global_thiz,
                                                      global_callback);
    callback->setIsMeListCallback(BS_TRUE);
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(object, g_ptr);
    obj->find(callback);
}

void deleteObject(JNIEnv *env, jobject thiz, jobject object, jobject jcallback) {
    jobject global_thiz = env->NewGlobalRef(thiz);
    jobject global_callback = env->NewGlobalRef(jcallback);
    MeObject *obj = (MeObject *) env->GetLongField(object, g_ptr);
    MeObjectCallback *callback = new MeObjectCallback(global_thiz, global_callback);
    callback->lock();
    obj->deleteObject(callback);
}

jstring download(JNIEnv *env, jobject thiz, jstring name, jstring jurl, jobject jcallback) {
    MeDownloadFile *file = new MeDownloadFile();

    const char *url = env->GetStringUTFChars(jurl, NULL);
    const char *fileName = env->GetStringUTFChars(name, NULL);
    file->setFilename(fileName);
    file->setDownloadUrl(url);

    jobject global_thiz = env->NewGlobalRef(thiz);
    jobject global_callback = env->NewGlobalRef(jcallback);

    MeAndroidHttpFileCallBack *callback = new MeAndroidHttpFileCallBack(
            file->className(), file);
    callback->lock();
    callback->setPara(global_thiz, global_callback);
    file->download(callback);
    env->ReleaseStringUTFChars(name, fileName);
    env->ReleaseStringUTFChars(jurl, url);
    return env->NewStringUTF(file->filePath());
}


void uploadFile(JNIEnv *env, jobject thiz, jstring type, jstring jpath, jobject jcallback) {
    const char *path = env->GetStringUTFChars(jpath, NULL);
    const char *typeName = env->GetStringUTFChars(type, NULL);

    MeUploadFile *file = new MeUploadFile();
    file->setUploadFilePath(path);
    file->put("type", typeName);

    jobject global_thiz = env->NewGlobalRef(thiz);
    jobject global_callback = env->NewGlobalRef(jcallback);
    GetAuthInfomationCallback *callback = new GetAuthInfomationCallback(
            "File", file, global_thiz, global_callback);
    callback->lock();
    file->getAuthInfomation(callback);
    env->ReleaseStringUTFChars(jpath, path);
    env->ReleaseStringUTFChars(type, typeName);
}

void uploadData(JNIEnv *env, jobject thiz, jstring jtype, jbyteArray jdata, jobject jcallback) {
    jobject global_thiz = env->NewGlobalRef(thiz);
    jobject global_callback = env->NewGlobalRef(jcallback);
    const char *type = env->GetStringUTFChars(jtype, NULL);
    unsigned char *data = (unsigned char *) env->GetPrimitiveArrayCritical(jdata, NULL);
    jsize size = env->GetArrayLength(jdata);
    MeUploadFile *file = new MeUploadFile();
    file->setUploadData(data, size);
    file->put("type", type);
    GetAuthInfomationCallback *callback = new GetAuthInfomationCallback(
            "File", file, global_thiz, global_callback);
    callback->lock();
    file->getAuthInfomation(callback);
    env->ReleasePrimitiveArrayCritical(jdata, data, NULL);
    env->ReleaseStringUTFChars(jtype, type);
}

void postData(JNIEnv *env, jobject thiz, jstring jurl, jbyteArray jData, jobject jcallBack) {
    jsize size = env->GetArrayLength(jData);
    const char *url = env->GetStringUTFChars(jurl, NULL);
    jobject global_thiz = env->NewGlobalRef(thiz);
    jobject global_callback = env->NewGlobalRef(jcallBack);
    MeObject *obj = new MeObject();
    MeAndroidHttpDataCallBack *callback = new MeAndroidHttpDataCallBack(
            obj->className(), obj);
    callback->lock();
    callback->setPara(global_thiz, global_callback);
    unsigned char *data = (unsigned char *) env->GetPrimitiveArrayCritical(jData, NULL);
    MeCloud::shareInstance()->postData(url, data, size, callback);
    env->ReleasePrimitiveArrayCritical(jData, data, NULL);
    env->ReleaseStringUTFChars(jurl, url);
}

void saveWithUrl(JNIEnv *env, jobject thiz, jstring jurl, jobject object, jobject jcallback) {
    const char *url = env->GetStringUTFChars(jurl, NULL);
    const char *param = "";
    if (object) {
        JSONObject *obj = (JSONObject *) env->GetLongField(object, g_ptr);
        param = obj->toString();
    }

    jobject g_callback = env->NewGlobalRef(jcallback);
    jobject g_thiz = env->NewGlobalRef(thiz);
    MeObjectCallback *meObjectCallback = new MeObjectCallback(g_thiz, g_callback);
    meObjectCallback->setIsMeListCallback(BS_FALSE);
    global_exception(env);
    MeCloud::shareInstance()->post(url, param, meObjectCallback);
    env->ReleaseStringUTFChars(jurl, url);
}

void getWithUrl(JNIEnv *env, jobject thiz, jstring jurl, jobject object, jobject jcallback) {
    const char *url = env->GetStringUTFChars(jurl, NULL);
    map<string, string> param;
    if (object) {
        JSONObject *obj = (JSONObject *) env->GetLongField(object, g_ptr);
        param = obj->stringdict();
    }
    jobject g_callback = env->NewGlobalRef(jcallback);
    jobject g_thiz = env->NewGlobalRef(thiz);
    MeObjectCallback *meObjectCallback = new MeObjectCallback(g_thiz, g_callback);
    meObjectCallback->setIsMeListCallback(BS_FALSE);
    meObjectCallback->setIsMeListCallback(BS_FALSE);
    global_exception(env);
    MeCloud::shareInstance()->get(url, param, meObjectCallback);
    env->ReleaseStringUTFChars(jurl, url);
}

//MeUser
jlong createUser(JNIEnv *env, jobject thiz) {
    setJSONObjectPtr(env, thiz);
    MeUser *user = new MeUser();
    return (jlong)user;
}

jlong createUserWithJSONObject(JNIEnv *env, jobject thiz, jlong jsonPtr) {
    setJSONObjectPtr(env, thiz);
    JSONObject *object = (JSONObject *)jsonPtr;
    MeUser *user = new MeUser(object);
    return (jlong) user;
}

jlong currentUser(JNIEnv *env, jobject thiz) {
    return (jlong) MeUser::currentUser();
}

void logout(JNIEnv *env, jobject thiz) {
    MeUser::currentUser()->logout();
}

void saveLoginUser(JNIEnv *env, jobject thiz, jlong userPtr) {
    MeObject *obj = (MeObject *) userPtr;
    MeUser::saveLoginUser(obj);
}

jstring encodePassword(JNIEnv *env, jobject thiz, jstring jusername, jstring jpassword) {
    const char *username = env->GetStringUTFChars(jusername, NULL);
    const char *password = env->GetStringUTFChars(jpassword, NULL);
    const char *newPassword = MeUser::encodePassword(username, password);
    jstring newJpassword = env->NewStringUTF(newPassword);
    delete newPassword;
    env->ReleaseStringUTFChars(jusername, username);
    env->ReleaseStringUTFChars(jpassword, password);
    return newJpassword;
}

jstring device(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF(MeUser::device());
}

// MeACL
void setPublicReadAccess(JNIEnv *env, jobject thiz) {
    MeACL *obj = (MeACL *) env->GetLongField(thiz, g_ptr);
    obj->setPublicReadAccess();
}

void setPublicWriteAccess(JNIEnv *env, jobject thiz) {
    MeACL *obj = (MeACL *) env->GetLongField(thiz, g_ptr);
    obj->setPublicWriteAccess();
}

void setRoleReadAccess(JNIEnv *env, jobject thiz, jstring jrole) {
    const char *role = env->GetStringUTFChars(jrole, NULL);
    MeACL *obj = (MeACL *) env->GetLongField(thiz, g_ptr);
    obj->setRoleReadAccess(role);
    env->ReleaseStringUTFChars(jrole, role);
}

void setRoleWriteAccess(JNIEnv *env, jobject thiz, jstring jrole) {
    const char *role = env->GetStringUTFChars(jrole, NULL);
    MeACL *obj = (MeACL *) env->GetLongField(thiz, g_ptr);
    obj->setRoleWriteAccess(role);
    env->ReleaseStringUTFChars(jrole, role);
}

void setRoleReadAccessMeRole(JNIEnv *env, jobject thiz, jlong objectPre) {
    MeACL *obj = (MeACL *) env->GetLongField(thiz, g_ptr);
    MeRole *role = (MeRole *) objectPre;
    obj->setRoleReadAccess(role);
}

void setRoleWriteAccessMeRole(JNIEnv *env, jobject thiz, jlong objectPre) {
    MeACL *obj = (MeACL *) env->GetLongField(thiz, g_ptr);
    MeRole *role = (MeRole *) objectPre;
    obj->setRoleWriteAccess(role);
}

void setUserReadAccess(JNIEnv *env, jobject thiz, jstring juserId) {
    const char *userid = env->GetStringUTFChars(juserId, NULL);
    MeACL *obj = (MeACL *) env->GetLongField(thiz, g_ptr);
    obj->setUserReadAccess(userid);
    env->ReleaseStringUTFChars(juserId, userid);
}

void setUserWriteAccess(JNIEnv *env, jobject thiz, jstring juserId) {
    const char *userid = env->GetStringUTFChars(juserId, NULL);
    MeACL *obj = (MeACL *) env->GetLongField(thiz, g_ptr);
    obj->setUserWriteAccess(userid);
    env->ReleaseStringUTFChars(juserId, userid);
}

void setUserReadAccessMeUser(JNIEnv *env, jobject thiz, jlong objectPre) {
    MeACL *obj = (MeACL *) env->GetLongField(thiz, g_ptr);
    MeUser *role = (MeUser *) objectPre;
    obj->setUserReadAccess(role);
}

void setUserWriteAccessMeUser(JNIEnv *env, jobject thiz, jlong objectPre) {
    MeACL *obj = (MeACL *) env->GetLongField(thiz, g_ptr);
    MeUser *role = (MeUser *) objectPre;
    obj->setUserReadAccess(role);
}

//MeJoinQuery
jlong createMeJoinQuery(JNIEnv *env, jobject thiz, jstring jclassName, jboolean jnestQuery) {
    setJSONObjectPtr(env, thiz);
    const char *className = env->GetStringUTFChars(jclassName, NULL);
    MeJoinQuery *obj = new MeJoinQuery(className, jnestQuery);
    env->ReleaseStringUTFChars(jclassName, className);
    return (jlong) obj;
}

void addSelectKeyWithJoin(JNIEnv *env, jobject thiz, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
    obj->addSelectKey(key);
    env->ReleaseStringUTFChars(jkey, key);
}


void addNotSelectKeyWithJoin(JNIEnv *env, jobject thiz, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
    obj->addNotSelectKey(key);
    env->ReleaseStringUTFChars(jkey, key);
}

void addAscendWithJoin(JNIEnv *env, jobject thiz, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
    obj->addNotSelectKey(key);
    env->ReleaseStringUTFChars(jkey, key);
}

void addDescendWithJoin(JNIEnv *env, jobject thiz, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
    obj->addDescend(key);
    env->ReleaseStringUTFChars(jkey, key);
}

void matchEqualTo(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *value = env->GetStringUTFChars(jvalue, NULL);
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
    obj->matchEqualTo(key, value);
    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jvalue, value);
}

void matchEqualToInt(JNIEnv *env, jobject thiz, jstring jkey, jint value) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
    obj->matchEqualTo(key, value);
    env->ReleaseStringUTFChars(jkey, key);
}

void matchGreater(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *value = env->GetStringUTFChars(jvalue, NULL);
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
    obj->matchGreater(key, value);
    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jvalue, value);
}

void matchGreaterToInt(JNIEnv *env, jobject thiz, jstring jkey, jint value) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
    obj->matchGreater(key, value);
    env->ReleaseStringUTFChars(jkey, key);
}

void matchLess(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *value = env->GetStringUTFChars(jvalue, NULL);
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
    obj->matchLess(key, value);
    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jvalue, value);
}

void matchLessToInt(JNIEnv *env, jobject thiz, jstring jkey, jint value) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
    obj->matchLess(key, value);
    env->ReleaseStringUTFChars(jkey, key);
}

void addForeignTable(JNIEnv *env, jobject thiz, jstring jfromTable, jstring jforeignKey,
                     jstring jlocalKey, jstring jdocument) {
    const char *fromTable = env->GetStringUTFChars(jfromTable, NULL);
    const char *foreignKey = env->GetStringUTFChars(jforeignKey, NULL);
    const char *localKey = env->GetStringUTFChars(jlocalKey, NULL);
    const char *document = env->GetStringUTFChars(jdocument, NULL);
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
    obj->addForeignTable(fromTable, foreignKey, localKey, document);
    env->ReleaseStringUTFChars(jfromTable, fromTable);
    env->ReleaseStringUTFChars(jforeignKey, foreignKey);
    env->ReleaseStringUTFChars(jlocalKey, localKey);
    env->ReleaseStringUTFChars(jdocument, document);
}

void addMeJoinQueryPtr(JNIEnv *env, jobject thiz, jlong joinQuery) {
    MeJoinQuery *query = (MeJoinQuery *) joinQuery;
    if (joinQuery != NULL) {
        MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
        obj->addMeJoinQuery(query);
    }
}

void addLimitWithJoin(JNIEnv *env, jobject thiz, jint count) {
    MeJoinQuery *obj = (MeJoinQuery *) env->GetLongField(thiz, g_ptr);
    obj->addLimit(count);
}

//MeAggregateObject
jlong createMeAggregateObject(JNIEnv *env, jobject thiz, jstring jclassName) {
    setJSONObjectPtr(env, thiz);
    const char *className = env->GetStringUTFChars(jclassName, NULL);
    MeAggregateObject *obj = new MeAggregateObject(className);
    env->ReleaseStringUTFChars(jclassName, className);
    return (jlong) obj;
}

void whereEqualToWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *value = env->GetStringUTFChars(jvalue, NULL);
    MeAggregateObject *obj = (MeAggregateObject *) env->GetLongField(thiz, g_ptr);
    obj->whereEqualTo(key, value);
    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jvalue, value);
}

void whereEqualToIntWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeAggregateObject *obj = (MeAggregateObject *) env->GetLongField(thiz, g_ptr);
    obj->whereEqualTo(key, jvalue);
    env->ReleaseStringUTFChars(jkey, key);
}

void whereNotEqualToWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *value = env->GetStringUTFChars(jvalue, NULL);
    MeAggregateObject *obj = (MeAggregateObject *) env->GetLongField(thiz, g_ptr);
    obj->whereNotEqualTo(key, value);
    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jvalue, value);
}

void whereNotEqualToIntWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeAggregateObject *obj = (MeAggregateObject *) env->GetLongField(thiz, g_ptr);
    obj->whereNotEqualTo(key, jvalue);
    env->ReleaseStringUTFChars(jkey, key);
}

void whereGreaterWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *value = env->GetStringUTFChars(jvalue, NULL);
    MeAggregateObject *obj = (MeAggregateObject *) env->GetLongField(thiz, g_ptr);
    obj->whereGreater(key, value);
    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jvalue, value);
}

void whereGreaterIntWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeAggregateObject *obj = (MeAggregateObject *) env->GetLongField(thiz, g_ptr);
    obj->whereGreater(key, jvalue);
    env->ReleaseStringUTFChars(jkey, key);
}

void whereLessWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jstring jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    const char *value = env->GetStringUTFChars(jvalue, NULL);
    MeAggregateObject *obj = (MeAggregateObject *) env->GetLongField(thiz, g_ptr);
    obj->whereLess(key, value);
    env->ReleaseStringUTFChars(jkey, key);
    env->ReleaseStringUTFChars(jvalue, value);
}

void whereLessIntWithAggregate(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeAggregateObject *obj = (MeAggregateObject *) env->GetLongField(thiz, g_ptr);
    obj->whereLess(key, jvalue);
    env->ReleaseStringUTFChars(jkey, key);
}

void setResponseKey(JNIEnv *env, jobject thiz, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeAggregateObject *obj = (MeAggregateObject *) env->GetLongField(thiz, g_ptr);
    obj->setResponseKey(key);
}

void setDistinctKey(JNIEnv *env, jobject thiz, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeAggregateObject *obj = (MeAggregateObject *) env->GetLongField(thiz, g_ptr);
    obj->setDistinctKey(key);
}

void setMethod(JNIEnv *env, jobject thiz, jint method) {
    MeAggregateObject *obj = (MeAggregateObject *) env->GetLongField(thiz, g_ptr);
    obj->setMethod((MEAGGREGATEMETHOD) method);
}

void setDevice(JNIEnv *env, jobject thiz, jobject jcontext) {
    mc::mc_set_device(env,thiz,jcontext);
}

void saveJSONToCache(JNIEnv *env, jobject thiz, jlong objectPre, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    JSONObject *obj = (JSONObject *) objectPre;
    JSONObject *newObject = new JSONObject(obj, false);
    MeCloud::shareInstance()->saveJSONToCache(newObject, key);
}
jlong getJSONFromCache(JNIEnv *env, jobject thiz, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    JSONObject *object = MeCloud::shareInstance()->getJSONFromCache(key);
    return (jlong) object;
}

void removeJSONFromCache(JNIEnv *env, jobject thiz, jstring jkey) {
    const char *key = env->GetStringUTFChars(jkey, NULL);
    MeCloud::shareInstance()->removeJSONFromCache(key);
}