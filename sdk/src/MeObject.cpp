/**
 * file :	MeObject.cpp
 * author :	bushaofeng
 * create :	2016-08-27 01:09
 * func : 
 * history:
 */

#include "MeObject.h"
#include "MeFile.h"
#include "bs.h"
#include "MeACL.h"
#include "McDevice.h"

#pragma --mark "构造函数"
MeObject::MeObject(){
    init();
}

MeObject::MeObject(const char* objectId, const char* className) {
    setClassName(className);
    snprintf(m_objectid, BS_UNIQUE_ID_LENGTH, "%s", objectId);
    put("_id", objectId);
}

MeObject::MeObject(MeObject* obj, bool auth):
JSONObject(obj, auth){
    assert(obj!=NULL);
    if (obj->objectId()) {
        snprintf(m_objectid, BS_UNIQUE_ID_LENGTH, "%s", obj->objectId());
    } else {
        init();
    }
    setClassName(obj->className());
    memset(m_db, 0, sizeof(m_db));
    
    for (map<string, MeObject*>::iterator iter = obj->m_object_map.begin(); iter!=obj->m_object_map.end(); iter++) {
        m_object_map[iter->first] = new MeObject(iter->second, false);
    }
    
    for (map<string, JSONArray*>::iterator iter = obj->m_array_map.begin(); iter!=obj->m_array_map.end(); iter++) {
        m_array_map[iter->first] = new JSONArray(iter->second, false);
    }
}

MeObject::MeObject(const char* className, JSONObject* json){
    setClassName(className);
    memset(m_db, 0, sizeof(m_db));
    if (json!=NULL) {
        copy(json);
    } else {
        init();
    }
}

#pragma 本地初始化objectId
void MeObject::init() {
    mc::guid(m_objectid);
}

const char* MeObject::className() {
    if (strlen(m_classname) > 0) {
        return m_classname;
    }
    
    const char *localClassName = JSONObject::stringValue("_class");
    if (!localClassName) {
        localClassName = "";
    }
    
    return localClassName;
}

#pragma --mark "参数设置"
void MeObject::setClassName(const char* className) {
    bs_strcpy(m_classname, sizeof(m_classname), className);
}

void MeObject::setDb(const char* db) {
    bs_strcpy(m_db, sizeof(m_db), db);
}

void MeObject::addUniqueKey(const char* key) {
    m_unique_key.push_back(string(key));
}

void MeObject::setACL(MeACL *acl) {
    objectAcl = *acl;
    if (stringValue("_id")==NULL) {
        JSONObject::put(ME_ACL_KEY, &objectAcl);
    } else{
        m_set_dirty.put(ME_ACL_KEY, &objectAcl);
    }
}

MeACL MeObject::getACL() {
    MeACL acl;
    JSONObject object = jsonValue("acl");
    JSONObject aclObject(object);
    acl.copy(&aclObject, true);
    return acl;
}

#pragma --mark "拷贝"
void MeObject::copy(JSONObject* obj, bool auth) {
    if (obj->has("_id")) {
        copySelf(obj, auth);
    }
    else{
        JSON::copy(obj, auth);
    }
}

void MeObject::copySelf(JSONObject *obj, bool auth) {
    if (!obj->has("_id") && strlen(m_objectid) < BS_UNIQUE_ID_EFFECT_LENGTH) {
        return;
    }
    
    clear();
    JSON::copy(obj, auth);
    
    if (obj->has("_id")) {
        snprintf(m_objectid, BS_UNIQUE_ID_LENGTH, "%s", obj->stringValue("_id"));
    }
}

#pragma --mark "反序列化"
bool MeObject::deserialize(const char* path){
    JSONObject obj;
    if(obj.deserialize(path)){
        copy(&obj);
        return true;
    }
    return false;
}

#pragma --mark "字段更新"
void MeObject::set(const char* name, MeObject* obj){
    if (stringValue("_id")==NULL) {
        obj->put("_sid", obj->objectId());
    }
    
    MeObject *newObject = new MeObject(obj, false);
    MeObject *oldObject = m_object_map[string(name)];
    if (oldObject) {
        delete oldObject;
    }
    m_object_map[string(name)] = newObject;
}

void MeObject::add(const char* name, MeObject* object) {
    JSONArray *newArray = m_array_map[string(name)];
    if (!newArray) {
        newArray = new JSONArray();
        m_array_map[string(name)] = newArray;
    }
    
    MeObject *newObject = new MeObject(object, false);
    newObject->put("_sid", object->objectId());
    newArray->append(newObject);
    m_child_classname[string(newObject->objectId())] = string(newObject->className());
}

void MeObject::set(const char* name, JSONArray* array, const char* childClassName) {
    JSONArray *newArray = m_array_map[string(name)];
    if (!newArray) {
        newArray = new JSONArray();
        m_array_map[string(name)] = newArray;
    }
    
    newArray->clear();
    for (int i = 0; i < array->size(); i++) {
        JSONObject json = array->jsonValue(i);
        MeObject *object = new MeObject();
        const char* id = object->objectId();
        m_child_classname[string(id)] = string(childClassName);
        object->copy(&json, false);
        object->put("_sid", id);
        newArray->append(object);
    }
}

MeObject MeObject::objectValue(const char* key) {
    map<string, MeObject*>::iterator iter = m_object_map.find(string(key));
    if (iter!=m_object_map.end()) {
        return *(iter->second);
    }
    
    MeObject *childObject = new MeObject();
    JSONObject object = JSONObject::jsonValue(key);
    if (!object.empty()) {
        childObject->copy(&object);
        m_object_map[string(key)] = childObject;
    }

    return *childObject;
}

JSONArray MeObject::arrayValue(const char* key) {
    map<string, JSONArray*>::iterator iter = m_array_map.find(string(key));
    if (iter!=m_array_map.end()) {
        return *(iter->second);
    }
    
    return JSONObject::arrayValue(key);
}

void MeObject::put(const char* name, const char* value){
    if (stringValue("_id") == NULL) {
        JSONObject::put(name, value);
    } else{
        m_set_dirty.put(name, value);
    }
}

void MeObject::put(const char* name, double value){
    if (stringValue("_id") == NULL) {
        JSONObject::put(name, value);
    } else{
        m_set_dirty.put(name, value);
    }
}

void MeObject::put(const char* name, float value){
    if (stringValue("_id") == NULL) {
        JSONObject::put(name, value);
    } else{
        m_set_dirty.put(name, value);
    }
}

void MeObject::put(const char* name, int value){
    if (stringValue("_id") == NULL) {
        JSONObject::put(name, value);
    } else{
        m_set_dirty.put(name, value);
    }
}

void MeObject::put(const char* name, long value){
    if (stringValue("_id") == NULL) {
        JSONObject::put(name, value);
    } else{
        m_set_dirty.put(name, value);
    }
}

void MeObject::put(const char* name, bool value){
    if (stringValue("_id") == NULL) {
        JSONObject::put(name, value);
    } else{
        m_set_dirty.put(name, value);
    }
}

#pragma --mark "字段增长"
void MeObject::increase(const char* name, int num){
    if (stringValue("_id") == NULL) {
        int v = intValue(name, 0);
        JSONObject::put(name, v+num);
    } else{
        m_inc_dirty.put(name, num);
    }
}

void MeObject::increase(const char* name, long num){
    if (stringValue("_id") == NULL) {
        long v = longValue(name, 0);
        JSONObject::put(name, v+num);
    } else{
        m_inc_dirty.put(name, num);
    }
}

void MeObject::increase(const char* name, float num){
    if (stringValue("_id") == NULL) {
        float v = floatValue(name, 0);
        JSONObject::put(name, v+num);
    } else{
        m_inc_dirty.put(name, num);
    }
}

#pragma --mark "构建m_url"
void MeObject::initPostUrl() {
    // 处理唯一索引
    char unique[512] = {0};
    for (int i=0; i<m_unique_key.size(); i++) {
        snprintf(unique, sizeof(unique)-strlen(unique)-1, "unique=%s&", m_unique_key[i].c_str());
    }
    
    if (strlen(unique) > 0) {
        // 去掉最后一个&
        unique[strlen(unique)-1] = '\0';
        snprintf(m_url, sizeof(m_url), "%s%s?%s", MeCloud::shareInstance()->classUrl(), m_classname, unique);
    } else{
        snprintf(m_url, sizeof(m_url), "%s%s", MeCloud::shareInstance()->classUrl(), m_classname);
    }
}

#pragma --mark "保存"
void MeObject::save(MeCallback* callback){
    if (callback!=NULL) {
        callback->m_object = this;
        callback->m_classname = m_classname;
    }
    
    if (m_object_map.empty() && m_array_map.empty()) {
        saveData(callback);
    } else {
        saveNestedData(callback);
    }
}

#pragma --mark "保存非嵌套类数据"
void MeObject::saveData(MeCallback* callback) {
    if (stringValue("_id") == NULL) {
        initPostUrl();
        postData(callback);
    } else {
        putData(callback);
    }
}

void MeObject::postData(MeCallback* callback) {
    // _sid为数据创造前的唯一id
    put("_sid", m_objectid);
    MeCloud::shareInstance()->post(m_url, toString(), callback);
}

void MeObject::putData(MeCallback* callback) {
    JSONObject *json = new JSONObject();
    if (!m_set_dirty.empty()) {
        json->put("$set", &m_set_dirty);
    }
    
    if (!m_inc_dirty.empty()>0) {
        json->put("$inc", &m_inc_dirty);
    }
    
    snprintf(m_url, sizeof(m_url), "%s%s/%s", MeCloud::shareInstance()->classUrl(), m_classname, m_objectid);
    MeCloud::shareInstance()->put(m_url, json->toString(), callback);
    delete json;
}

#pragma --mark "保存嵌套类数据"
void MeObject::saveNestedData(MeCallback* callback) {
    JSONArray *array = new JSONArray();
    map<string, string> nestObjectMap;
    for (map<string, MeObject*>::iterator iter = m_object_map.begin(); iter!=m_object_map.end(); iter++) {
        MeObject *obj = iter->second;
        if (obj->stringValue("_id") == NULL) {
            JSONObject *classObj = new JSONObject();
            classObj->setDestruct(false);
            classObj->put(obj->className(), obj);
            array->append(classObj);
            delete classObj;
        }
      
        nestObjectMap[iter->first] = obj->objectId();
    }
  
    map<string, JSONArray*> nestArrayMap;
    for (map<string, JSONArray*>::iterator iter = m_array_map.begin(); iter!=m_array_map.end(); iter++) {
        JSONArray *objectArray = iter->second;
        JSONArray *nestArray = new JSONArray();
        for (int i = 0; i < objectArray->size(); i++) {
            JSONObject item = objectArray->jsonValue(i);
            const char *objectId = item.stringValue("_sid");
            const char *childClassname = m_child_classname[string(objectId)].c_str();
            if (item.stringValue("_id") == NULL) {
                JSONObject *classObj = new JSONObject();
                classObj->setDestruct(false);
                classObj->put(childClassname, &item);
                array->append(classObj);
                delete classObj;
            }
          
            nestArray->append(objectId);
        }
        
        nestArrayMap[iter->first] = nestArray;
    }
    
    if (stringValue("_id") == NULL) {
        initPostUrl();
        for (map<string, string>::iterator iter = nestObjectMap.begin(); iter!=nestObjectMap.end(); iter++) {
            JSONObject::put(iter->first.c_str(), iter->second.c_str());
        }
        for (map<string, JSONArray*>::iterator iter = nestArrayMap.begin(); iter!=nestArrayMap.end(); iter++) {
            JSONObject::put(iter->first.c_str(), iter->second);
        }
        postNestedData(callback, array);
    } else {
        for (map<string, string >::iterator iter = nestObjectMap.begin(); iter!=nestObjectMap.end(); iter++) {
            m_set_dirty.put(iter->first.c_str(), iter->second.c_str());
        }
        for (map<string, JSONArray*>::iterator iter = nestArrayMap.begin(); iter!=nestArrayMap.end(); iter++) {
            JSONObject::put(iter->first.c_str(), iter->second);
        }
        putNestedData(callback, array);
    }
    
    delete array;
    for (map<string, JSONArray*>::iterator iter = nestArrayMap.begin(); iter!=nestArrayMap.end(); iter++) {
        JSONArray* jsonArray = iter->second;
        delete jsonArray;
    }
    nestObjectMap.clear();
    nestArrayMap.clear();
}

void MeObject::postNestedData(MeCallback* callback, JSONArray *array) {
    // _sid为数据创造前的唯一id
    put("_sid", m_objectid);
    JSONObject *json = new JSONObject();
    json->copy(this, false);
    array->append(json);
    
    if (array->size() <= 1) {
        MeCloud::shareInstance()->post(m_url, array->jsonValue(0).toString(), callback);
    } else {
        MeCloud::shareInstance()->post(m_url, array->toString(), callback);
    }
    
    json->setDestruct(false);
    delete json;
}

void MeObject::putNestedData(MeCallback* callback, JSONArray *array) {
    JSONObject *json = new JSONObject();
    json->setDestruct(false);
    json->put("$set", &m_set_dirty);
    
    if (!m_inc_dirty.empty() > 0) {
        json->put("$inc", &m_inc_dirty);
    }
    
    array->append(json);
    
    snprintf(m_url, sizeof(m_url), "%s%s/%s", MeCloud::shareInstance()->classUrl(), m_classname, m_objectid);
    if (array->size() <= 1) {
        MeCloud::shareInstance()->put(m_url, array->jsonValue(0).toString(), callback);
    } else {
        MeCloud::shareInstance()->put(m_url, array->toString(), callback);
    }
    
    
    delete json;
}

void MeObject::deleteObject(MeCallback* callback) {
    if (stringValue("_id") != NULL) {
        snprintf(m_url, sizeof(m_url), "%s%s/%s", MeCloud::shareInstance()->classUrl(), m_classname ,m_objectid);
        MeCloud::shareInstance()->del(m_url, callback);
    } else {
        char message[] = "无效数据";
        callback->done(HTTP_OK, BS_INVALID, message);
    }
}

#pragma --mark "清空与虚构"
void MeObject::clear(){
    JSONObject::clear();
    m_set_dirty.clear();
    m_inc_dirty.clear();
    clearMap();
}

void MeObject::clearMap() {
    for (map<string, MeObject*>::iterator iter = m_object_map.begin(); iter!=m_object_map.end(); iter++) {
        MeObject* obj = iter->second;
        delete obj;
    }
    
    for (map<string, JSONArray*>::iterator iter = m_array_map.begin(); iter!=m_array_map.end(); iter++) {
        JSONArray* array = iter->second;
        delete array;
    }
    
    m_object_map.clear();
    m_array_map.clear();
    m_child_classname.clear();
}

MeObject::~MeObject() {
    clearMap();
}
