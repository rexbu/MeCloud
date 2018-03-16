/**
 * file :	MeQuery.cpp
 * author :	bushaofeng
 * create :	2016-08-27 01:09
 * func : 
 * history:
 */

#include "MeQuery.h"
#include <algorithm>

MeQuery::MeQuery(const char* className){
    bs_strcpy(m_classname, sizeof(m_classname), className);
}

void MeQuery::whereEqualTo(const char* key, const char* val){
    put(key, val);
}
void MeQuery::whereNotEqualTo(const char* key, const char* val){
    JSONObject obj;
    obj.put("$ne", val);
    put(key, &obj);
}
void MeQuery::whereEqualTo(const char* key, int val){
    put(key, val);
}
void MeQuery::whereNotEqualTo(const char* key, int val){
    JSONObject obj;
    obj.put("$ne", val);
    put(key, &obj);
}
void MeQuery::selectKeys(const char* keys[], int num){
    for (int i=0; i<num; i++) {
        addSelectKey(keys[i]);
    }
}

void MeQuery::addSelectKey(const char* key){
    m_select_keys.put(key, 1);
}

void MeQuery::addNotSelectKey(const char* key){
    m_select_keys.put(key, 0);
}

void MeQuery::get(const char* objectId, MeCallback* callback){
    snprintf(m_url+strlen(m_url), sizeof(m_url)-strlen(m_url), "/%s", objectId);
}

void MeQuery::find(MeCallback* callback){
    if (!m_select_keys.empty()) {
        snprintf(m_url, sizeof(m_url)-1, "%s%s?where=%s&keys=%s", MeCloud::shareInstance()->classUrl(), m_classname, toString(), m_select_keys.toString());
    }
    else{
        snprintf(m_url, sizeof(m_url)-1, "%s%s?where=%s", MeCloud::shareInstance()->classUrl(), m_classname, toString());
    }
    
    string str = m_url;
    auto itor = remove(str.begin(), str.end(), '\n');
    str.erase(itor, str.end());
    MeCloud::shareInstance()->get(str.c_str(), callback);
}
