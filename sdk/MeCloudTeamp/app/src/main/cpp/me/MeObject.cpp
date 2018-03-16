/**
 * file :	MeObject.cpp
 * author :	bushaofeng
 * create :	2016-08-27 01:09
 * func : 
 * history:
 */

#include "MeObject.h"
#include "MeFile.h"
#include "../mc/bs/bs.h"
#include "MeACL.h"

#pragma --mark "构造函数"
MeObject::MeObject(){
    m_objectid = NULL;
}

MeObject::MeObject(MeObject* obj, bool auth):
JSONObject(obj, auth){
    assert(obj!=NULL);
    m_objectid = NULL;
    
    setClassName(obj->className());
    memset(m_db, 0, sizeof(m_db));
    
    m_object_map = obj->m_object_map;
}

MeObject::MeObject(const char* className, JSONObject* json){
    m_objectid = NULL;
    
    setClassName(className);
    memset(m_db, 0, sizeof(m_db));
    if (json!=NULL) {
        copy(json);
    }
}

#pragma --mark "参数设置"
void MeObject::setClassName(const char* className){
    bs_strcpy(m_classname, sizeof(m_classname), className);
}

void MeObject::setDb(const char* db){
    bs_strcpy(m_db, sizeof(m_db), db);
}

void MeObject::addUniqueKey(const char* key){
    m_unique_key.push_back(string(key));
}

void MeObject::setACL(MeACL* acl){
    if (m_objectid==NULL) {
        JSONObject::put(ME_ACL_KEY, acl);
    }
    else{
        m_set_dirty.put(ME_ACL_KEY, acl);
    }
}

MeACL MeObject::getACL(){
    MeACL acl;
    JSONObject object = jsonValue("acl");
    JSONObject aclObject(object);
    acl.copy(&aclObject, true);
    return acl;
}

#pragma --mark "拷贝"
void MeObject::copy(JSONObject* obj, bool auth){
    if (obj->has("_id")) {
        copySelf(obj, auth);
    }
    else{
        JSON::copy(obj, auth);
    }
}

void MeObject::copySelf(JSONObject *obj, bool auth){
    if (!obj->has("_id")) {
        return;
    }
    
    clear();
    JSON::copy(obj, auth);
    
    cJSON *c= m_root->child;
    while (c){
        JSONObject json(c);
        if (c->type==cJSON_Object && json.has("_type") && strcmp(json.stringValue("_type"), "pointer")==0) {
            MeObject* object;
            JSONObject content(cJSON_GetObjectItem(c, "_content"));
            const char* classname = json.stringValue("_class");
            if (strcmp(classname, "MeFile")==0) {
                object = new MeFile(&content);
            }
            else{
                object = new MeObject(classname, &content);
            }
            // 不转移权限，仍然存储在m_root中
            object->setDestruct(false);
            
            if (json.has("_db")) {
                object->setDb(json.stringValue("_db"));
            }
            
            m_object_map[string(c->string)] = object;
        }
        c=c->next;
    }
    
    m_objectid = stringValue("_id");
}

#pragma --mark "保存"
void MeObject::save(MeCallback* callback){
    if (callback!=NULL) {
        callback->m_object = this;
        callback->m_classname = m_classname;
    }
    
    if (m_objectid == NULL) {
        // 处理唯一索引
        char unique[512] = {0};
        for (int i=0; i<m_unique_key.size(); i++) {
            snprintf(unique, sizeof(unique)-strlen(unique)-1, "unique=%s&", m_unique_key[i].c_str());
        }
        if (strlen(unique) > 0) {
            // 去掉最后一个&
            unique[strlen(unique)-1] = '\0';
            snprintf(m_url, sizeof(m_url), "%s%s?%s", MeCloud::shareInstance()->classUrl(), m_classname, unique);
        }
        else{
            snprintf(m_url, sizeof(m_url), "%s%s", MeCloud::shareInstance()->classUrl(), m_classname);
        }
        
        MeCloud::shareInstance()->post(m_url, toString(), callback);
    }
    else{
        const char* set = NULL;
        const char* inc = NULL;
        char buf[1024];
        char* buffer = buf;
        if (!m_set_dirty.empty()) {
            set = m_set_dirty.toString();
        }
        if (!m_inc_dirty.empty()>0) {
            inc = m_inc_dirty.toString();
        }
        
        size_t size = 0;
        if (set!=NULL) {
            size += strlen(set);
        }
        if (inc!=NULL) {
            size += strlen(inc);
        }
        
        if (size>sizeof(buf)-20) {
            buffer = (char*)malloc(strlen(set)+strlen(inc)+20);
        }
        if (set!=NULL && inc!=NULL) {
            sprintf(buffer, "{\"$set\":%s,\"$inc\":%s}", set, inc);
        }
        else if (set!=NULL){
            sprintf(buffer, "{\"$set\":%s}", set);
        }
        else if (inc!=NULL){
            sprintf(buffer, "{\"$inc\":%s}", inc);
        }
        
        //TODO: toString 查找name出错
//        JSONObject setObj;
//        if (m_set_dirty.size()>0) {
//            setObj.put("$set", m_set_dirty);
//        }
//        if (m_inc_dirty.size()>0) {
//            setObj.put("$inc", m_inc_dirty);
//        }
        
        snprintf(m_url, sizeof(m_url), "%s%s/%s", MeCloud::shareInstance()->classUrl(), m_classname, m_objectid);
        MeCloud::shareInstance()->put(m_url, buffer, callback);
        if (buffer!=buf) {
            free(buffer);
        }
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

void MeObject::put(const char* name, MeObject* obj){
// TODO: 暂未考虑put MeObject
//    JSONObject::put(iter->name.GetString(), ME_TYPE_POINTER);
//    m_object_map[string(iter->name.GetString())] = object;
}

MeObject* MeObject::objectValue(const char* key){
    map<string, MeObject*>::iterator iter = m_object_map.find(string(key));
    if (iter!=m_object_map.end()) {
        return iter->second;
    }
    return NULL;
}


void MeObject::put(const char* name, const char* value){
    if (m_objectid==NULL) {
        JSONObject::put(name, value);
    }
    else{
        m_set_dirty.put(name, value);
    }
}

void MeObject::put(const char* name, double value){
    if (m_objectid==NULL) {
        JSONObject::put(name, value);
    }
    else{
        m_set_dirty.put(name, value);
    }
}

void MeObject::put(const char* name, float value){
    if (m_objectid==NULL) {
        JSONObject::put(name, value);
    }
    else{
        m_set_dirty.put(name, value);
    }
}

void MeObject::put(const char* name, int value){
    if (m_objectid==NULL) {
        JSONObject::put(name, value);
    }
    else{
        m_set_dirty.put(name, value);
    }
}

void MeObject::put(const char* name, long value){
    if (m_objectid==NULL) {
        JSONObject::put(name, value);
    }
    else{
        m_set_dirty.put(name, value);
    }
}

void MeObject::put(const char* name, bool value){
    if (m_objectid==NULL) {
        JSONObject::put(name, value);
    }
    else{
        m_set_dirty.put(name, value);
    }
}

#pragma --mark "字段增长"
void MeObject::increase(const char* name, int num){
    if (m_objectid==NULL) {
        int v = intValue(name, 0);
        JSONObject::put(name, v+num);
    }
    else{
        m_inc_dirty.put(name, num);
    }
}

void MeObject::increase(const char* name, long num){
    if (m_objectid==NULL) {
        long v = longValue(name, 0);
        JSONObject::put(name, v+num);
    }
    else{
        m_inc_dirty.put(name, num);
    }
}

void MeObject::increase(const char* name, float num){
    if (m_objectid==NULL) {
        float v = floatValue(name, 0);
        JSONObject::put(name, v+num);
    }
    else{
        m_inc_dirty.put(name, num);
    }
}

#pragma --mark "清空与虚构"
void MeObject::clear(){
    JSONObject::clear();
    m_set_dirty.clear();
    m_inc_dirty.clear();
}
MeObject::~MeObject(){
    for (map<string, MeObject*>::iterator iter = m_object_map.begin(); iter!=m_object_map.end(); iter++) {
        MeObject* obj = iter->second;
        delete obj;
    }
    m_object_map.clear();
}
