/**
 * file :	MeACL.cpp
 * author :	Rex
 * create :	2017-07-18 17:57
 * func : 
 * history:
 */

#include "MeACL.h"
#include "MeRole.h"
#include "MeUser.h"

#define ME_ACL_WRITE_ACCSS          "write"               // 写权限可key
#define ME_ACL_READ_ACCSS           "read"                // 读权限的key
#define ME_ACL_DELETE_ACCSS         "delete"              // 读权限的key
#define ME_ACL_PUBLIC_IDENTIFIER    "*"                   // 身份key
// 三面三个key请参考下面注解
/*
 存储格式
 {
    "*":{"read": true},
    "user1_id": {"write": true},
    "role:role1": {"read": true}
 }
*/

MeACL::MeACL():JSONObject() {
}

//全部执行深拷贝
MeACL::MeACL(MeACL* acl):JSONObject(acl, false) {
    for (map<string, JSONObject*>::iterator iter = acl->m_object_map.begin(); iter!=acl->m_object_map.end(); iter++) {
        m_object_map[iter->first] = new JSONObject(iter->second, false);
    }
}

MeACL& MeACL::operator=(const MeACL &acl) {
    if (m_root!=NULL) {
        cJSON_Delete(m_root);
    }
    
    m_root = cJSON_Duplicate(acl.getCJSON(), 1);
    m_auto_destruct  = true;
    
    clear();
    
    for (map<string, JSONObject*>::const_iterator iter = acl.m_object_map.begin(); iter!=acl.m_object_map.end(); iter++) {
        JSONObject* obj = iter->second;
        m_object_map[iter->first] = new JSONObject(obj, false);
    }
    
    return *this;
}

#pragma --mark 公开权限
void MeACL::setPublicReadAccess(){
    setACLAccess(ME_ACL_READ_ACCSS, ME_ACL_PUBLIC_IDENTIFIER);
}

void MeACL::setPublicWriteAccess(){
    setACLAccess(ME_ACL_WRITE_ACCSS, ME_ACL_PUBLIC_IDENTIFIER);
}

#pragma --mark 角色权限
void MeACL::setRoleReadAccess(const char* role){
    setACLRoleAccess(ME_ACL_READ_ACCSS, role);
}

void MeACL::setRoleWriteAccess(const char* role){
    setACLRoleAccess(ME_ACL_WRITE_ACCSS, role);
}

void MeACL::setRoleDeleteAccess(const char* role) {
    setACLRoleAccess(ME_ACL_DELETE_ACCSS, role);
}

void MeACL::setRoleReadAccess(MeRole* role){
    setRoleReadAccess(role->stringValue(ME_ROLE_KEY));
}

void MeACL::setRoleWriteAccess(MeRole* role){
    setRoleWriteAccess(role->stringValue(ME_ROLE_KEY));
}

void MeACL::setRoleDeleteAccess(MeRole* role) {
    setRoleDeleteAccess(role->stringValue(ME_ROLE_KEY));
}

void MeACL::setACLRoleAccess(const char* name, const char* role, bool access){
    char *rolename = (char *)malloc(strlen(ME_ROLE_KEY) + strlen(role) + 2);
    strcpy(rolename, ME_ROLE_KEY);
    strcat(rolename, ":");
    strcat(rolename, role);
    setACLAccess(name, rolename);
    free(rolename);
}

#pragma --mark 用户权限
void MeACL::setUserReadAccess(const char* userId){
    setACLAccess(ME_ACL_READ_ACCSS, userId);
}

void MeACL::setUserWriteAccess(const char* userId){
    setACLAccess(ME_ACL_WRITE_ACCSS, userId);
}

void MeACL::setUserDeleteAccess(const char* userId) {
    setACLAccess(ME_ACL_DELETE_ACCSS, userId);
}

void MeACL::setUserReadAccess(MeUser* user){
    setUserReadAccess(user->objectId());
}

void MeACL::setUserWriteAccess(MeUser* user){
    setUserWriteAccess(user->objectId());
}

void MeACL::setUserDeleteAccess(MeUser* user) {
    setUserDeleteAccess(user->objectId());
}

#pragma --mark 通用接口
void MeACL::setACLAccess(const char* name, const char* identifier, bool access){
    JSONObject *accessObject = m_object_map[identifier];
    if (!has(identifier)) {
        accessObject = new JSONObject();
        m_object_map[identifier] = accessObject;
    }
    
    accessObject->put(name, access);
    put(identifier, accessObject);
}

void MeACL::clear() {
    for (map<string, JSONObject*>::iterator iter = m_object_map.begin(); iter!=m_object_map.end(); iter++) {
        JSONObject* obj = iter->second;
        delete obj;
    }
    
    m_object_map.clear();
}

MeACL::~MeACL(){
    clear();
}
