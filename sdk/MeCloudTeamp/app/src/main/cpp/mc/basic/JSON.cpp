/**
 * file :	JSON.cpp
 * author :	Rex
 * create :	2016-09-19 18:53
 * func : 
 * history:
 */

#include "JSON.h"
#include "McZip.h"

using namespace mc;

#pragma --mark "JSON"
JSON::JSON(){
    m_root = NULL;
    m_auto_destruct = true;
};

JSON::~JSON(){
    if (m_auto_destruct) {
        cJSON_Delete(m_root);
    }
}

const char* JSON::toString(){
    m_json_string = cJSON_Print(m_root);
    return m_json_string.c_str();
}

bool JSON::serialize(const char* path){
    Crypt crypt;
    
    return crypt.encryptToFile(path, (byte*)toString(), (uint32_t)m_json_string.size()) > 0;
}

bool JSON::deserialize(const char* path){
    Crypt crypt;
    state_t st = crypt.decryptFromFile(path);
    if (st <= 0) {
        return false;
    }
    
    parse((const char*)crypt.bytes());
    return true;
}

bool JSON::parse(const char* buf){
    if (m_root!=NULL && m_auto_destruct) {
        cJSON_Delete(m_root);
    }
    
    m_root = cJSON_Parse(buf);
    m_auto_destruct = true;
    if (m_root != NULL) {
        return true;
    }
    
    return false;
}

void JSON::copy(JSON* obj, bool auth){
    if (obj!=NULL && m_root!=NULL) {
        cJSON_Delete(m_root);
    }
    
    // 转移权限
    if (auth) {
        m_root = obj->getCJSON();
        m_auto_destruct = true;
        obj->setDestruct(false);
    }
    else{
        m_root = cJSON_Duplicate(obj->getCJSON(), 1);
        m_auto_destruct  = true;
    }
}

#pragma --mark "JSONObject"
JSONObject::JSONObject(){
    m_root = cJSON_CreateObject();
    m_auto_destruct = true;
}

JSONObject::JSONObject(cJSON* root, bool copy){
    if (root->type == cJSON_Object) {
        if (copy) {
            m_root = cJSON_Duplicate(root, 1);
            m_auto_destruct = true;
        }
        else{
            m_root = root;
            m_auto_destruct = false;
        }
    }
    else{
        m_root = cJSON_CreateObject();
        m_auto_destruct = true;
    }
    
}

JSONObject::JSONObject(const char* buf){
    cJSON* root = cJSON_Parse(buf);
    if (root!=NULL && root->type==cJSON_Object) {
        m_root = root;
    }
    else{
        if(root!=NULL){
            cJSON_Delete(root);
        }
        m_root = cJSON_CreateObject();
    }
    m_auto_destruct = true;
}

// 使用JSON初始化, 是否转移cJSON的释放权限
JSONObject::JSONObject(JSON* cjson, bool transAuth){
    if (cjson->getCJSON()->type!=cJSON_Object) {
        m_root = cJSON_CreateObject();
        m_auto_destruct = true;
        return;
    }
    
    JSON::copy(cjson, transAuth);
}

JSONObject::JSONObject(const JSONObject& cjson){
    m_root = cJSON_Duplicate(cjson.m_root, 1);
    m_auto_destruct = true;
}

void JSONObject::put(const char* name, const char* value){
    cJSON* item = cJSON_GetObjectItem(m_root, name);
    if (item==NULL) {
        cJSON_AddStringToObject(m_root, name, value);
    }
    else{
        item->type=cJSON_String;
        if (item->valuestring) {
            free(item->valuestring);
            item->valuestring = NULL;
        }
        
        size_t len = strlen(value) + 1;
        char* copy = (char*)malloc(len);
        if (copy==NULL) {
            err_log("Error: malloc failed!!!");
            return;
        }
        
        memcpy(copy, value, len);
        item->valuestring = copy;
    }
}

void JSONObject::put(const char* name, double value){
    cJSON* item = cJSON_GetObjectItem(m_root, name);
    if (item==NULL) {
        cJSON_AddNumberToObject(m_root, name, value);
    }
    else{
        cJSON_SetNumberValue(item, value);
    }
}

void JSONObject::put(const char* name, int value){
    cJSON* item = cJSON_GetObjectItem(m_root, name);
    if (item==NULL) {
        cJSON_AddNumberToObject(m_root, name, value);
    }
    else{
        cJSON_SetNumberValue(item, value);
    }
}

void JSONObject::put(const char* name, long value){
    cJSON* item = cJSON_GetObjectItem(m_root, name);
    if (item==NULL) {
        cJSON_AddNumberToObject(m_root, name, value);
    }
    else{
        cJSON_SetNumberValue(item, value);
    }
}

void JSONObject::put(const char* name, float value){
    cJSON* item = cJSON_GetObjectItem(m_root, name);
    if (item==NULL) {
        cJSON_AddNumberToObject(m_root, name, value);
    }
    else{
        cJSON_SetNumberValue(item, value);
    }
}

void JSONObject::put(const char* name, bool value){
    cJSON* item = cJSON_GetObjectItem(m_root, name);
    if (item==NULL) {
        if (value) {
            cJSON_AddTrueToObject(m_root, name);
        }
        else{
            cJSON_AddFalseToObject(m_root, name);
        }
    }
    else{
        item->type = value;
    }
}

void JSONObject::putNull(const char* name){
    cJSON* item = cJSON_GetObjectItem(m_root, name);
    if (item==NULL) {
        cJSON_AddNullToObject(m_root, name);
    }
    else{
        item->type=cJSON_NULL;
        if (item->valuestring) {
            free(item->valuestring);
            item->valuestring = NULL;
        }
    }
}

// 如果不转移释放权限，则会拷贝一份
void JSONObject::put(const char* name, JSON* json){
    cJSON* item = cJSON_GetObjectItem(m_root, name);
    if (item==NULL) {
        cJSON_AddItemReferenceToObject(m_root, name, json->getCJSON());
    }
    else{
        if (item->child) {
            cJSON_Delete(item->child);
            item->child = NULL;
        }
        if (item->valuestring) {
            free(item->valuestring);
            item->valuestring = NULL;
        }
        item->child = json->getCJSON();
        json->setDestruct(false);
    }
}

void JSONObject::put(const char* name, cJSON* json){
    cJSON* item = cJSON_GetObjectItem(m_root, name);
    if (item==NULL) {
        cJSON_AddItemToObject(m_root, name, json);
    }
    else{
        if (item->child) {
            cJSON_Delete(item->child);
            item->child = NULL;
        }
        if (item->valuestring) {
            free(item->valuestring);
            item->valuestring = NULL;
        }
        item->child = cJSON_Duplicate(json, 1);
    }
}

bool JSONObject::has(const char* name){
    return cJSON_GetObjectItem(m_root, name) != NULL;
}

int JSONObject::intValue(const char* name, int dv){
    cJSON* json = cJSON_GetObjectItem(m_root, name);
    if (json != NULL && json->type==cJSON_Number) {
        return json->valueint;
    }
    else{
        return dv;
    }
}

long JSONObject::longValue(const char* name, long dv){
    cJSON* json = cJSON_GetObjectItem(m_root, name);
    if (json != NULL && json->type==cJSON_Number) {
        return json->valuedouble;
    }
    else{
        return dv;
    }
}

float JSONObject::floatValue(const char* name, float dv){
    cJSON* json = cJSON_GetObjectItem(m_root, name);
    if (json != NULL && json->type==cJSON_Number) {
        return json->valuedouble;
    }
    else{
        return dv;
    }
}

double JSONObject::doubleValue(const char* name, double dv){
    cJSON* json = cJSON_GetObjectItem(m_root, name);
    if (json != NULL && json->type==cJSON_Number) {
        return json->valuedouble;
    }
    else{
        return dv;
    }
}

bool_t JSONObject::boolValue(const char* name){
    cJSON* json = cJSON_GetObjectItem(m_root, name);
    if (json != NULL && json->type==cJSON_True) {
        return BS_TRUE;
    }
    else if(json != NULL && json->type==cJSON_True){
        return BS_FALSE;
    }
    else{
        return BS_NOTBOOL;
    }
}

const char* JSONObject::stringValue(const char* name){
    cJSON* json = cJSON_GetObjectItem(m_root, name);
    if (json != NULL && json->type==cJSON_String) {
        return json->valuestring;
    }
    else{
        return NULL;
    }
}

JSONObject JSONObject::jsonValue(const char* name){
    cJSON* json = cJSON_GetObjectItem(m_root, name);
    if (json==NULL || (json->type!=cJSON_Object && (json->type^cJSON_IsReference)!=cJSON_Object)) {
        return JSONObject();
    }
    else{
        if ((json->type^cJSON_IsReference)==cJSON_Object){
            json->type ^= cJSON_IsReference;
        }
        // 可以考虑引用计数
        return JSONObject(json);
    }
}

JSONArray JSONObject::arrayValue(const char* name){
    cJSON* json = cJSON_GetObjectItem(m_root, name);
    if (json==NULL || json->type!=cJSON_Array) {
        return JSONArray();
    }
    else{
        // 可以考虑引用计数
        return JSONArray(json);
    }
}

std::vector<std::string> JSONObject::keys(){
    std::vector<std::string> ks;
    cJSON* obj = m_root->child;
    while (obj!=NULL) {
        std::string s = obj->string;
        ks.push_back(s);
        obj = obj->next;
    }
    
    return ks;
}

#pragma --mark "JSONArray"
JSONArray::JSONArray(){
    m_root = cJSON_CreateArray();
    m_auto_destruct = true;
}

JSONArray::JSONArray(cJSON* root){
    if (root->type == cJSON_Array) {
        m_root = cJSON_Duplicate(root, 1);
    }
    else{
        m_root = cJSON_CreateObject();
    }
    
    m_auto_destruct = true;
}

JSONArray::JSONArray(const char* buf){
    cJSON* root = cJSON_Parse(buf);
    if (root!=NULL && root->type==cJSON_Array) {
        m_root = root;
    }
    else{
        if(root!=NULL){
            cJSON_Delete(root);
        }
        m_root = cJSON_CreateArray();
    }
    m_auto_destruct = true;
}

// 使用JSON初始化, 是否转移cJSON的释放权限
JSONArray::JSONArray(JSON* cjson, bool transAuth){
    if (cjson->getCJSON()->type!=cJSON_Array) {
        m_root = cJSON_CreateArray();
        m_auto_destruct = true;
        return;
    }
    
    JSON::copy(cjson, transAuth);
}

JSONArray::JSONArray(const JSONArray& cjson){
    m_root = cJSON_Duplicate(cjson.m_root, 1);
    m_auto_destruct = true;
}

void JSONArray::append(const char* value){
    cJSON_AddItemToArray(m_root, cJSON_CreateString(value));
}
void JSONArray::append(int value){
    cJSON_AddItemToArray(m_root, cJSON_CreateNumber(value));
}
void JSONArray::append(bool value){
    if (value) {
        cJSON_AddItemToArray(m_root, cJSON_CreateTrue());
    }
    else{
        cJSON_AddItemToArray(m_root, cJSON_CreateFalse());
    }
}
void JSONArray::append(float value){
    cJSON_AddItemToArray(m_root, cJSON_CreateNumber(value));
}

void JSONArray::append(double value){
    cJSON_AddItemToArray(m_root, cJSON_CreateNumber(value));
}

void JSONArray::append(JSON* value){
    cJSON_AddItemToArray(m_root, value->getCJSON());
}

int* JSONArray::intValue(uint32_t index){
    cJSON* json = cJSON_GetArrayItem(m_root, index);
    if (json!=NULL && json->type==cJSON_Number) {
        return &json->valueint;
    }
    else{
        return NULL;
    }
}
double* JSONArray::doubleValue(uint32_t index){
    cJSON* json = cJSON_GetArrayItem(m_root, index);
    if (json!=NULL && json->type==cJSON_Number) {
        return &json->valuedouble;
    }
    else{
        return NULL;
    }
}
bool_t JSONArray::boolValue(uint32_t index){
    cJSON* json = cJSON_GetArrayItem(m_root, index);
    if (json != NULL && json->type==cJSON_True) {
        return BS_TRUE;
    }
    else if(json != NULL && json->type==cJSON_True){
        return BS_FALSE;
    }
    else{
        return BS_NOTBOOL;
    }
}
char* JSONArray::stringValue(uint32_t index){
    cJSON* json = cJSON_GetArrayItem(m_root, index);
    if (json!=NULL && json->type==cJSON_String) {
        return json->valuestring;
    }
    else{
        return NULL;
    }
}

JSONObject JSONArray::jsonValue(uint32_t index){
    cJSON* json = cJSON_GetArrayItem(m_root, index);
    if (json!=NULL && json->type==cJSON_Object) {
        return JSONObject(json);
    }
    else{
        return JSONObject();
    }
}

JSONArray JSONArray::arrayValue(uint32_t index){
    cJSON* json = cJSON_GetArrayItem(m_root, index);
    if (json!=NULL && json->type==cJSON_Array) {
        return JSONArray(json);
    }
    else{
        return JSONArray();
    }
}
