/**
 * file :	MeACL.cpp
 * author :	Rex
 * create :	2017-07-18 17:57
 * func : 
 * history:
 */

#include "MeACL.h"

#define ME_ACL_WRITE_ACCSS          "write"              // 写权限可key
#define ME_ACL_READ_ACCSS           "read"                // 读权限的key
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

#pragma 公开权限
void MeACL::setPublicReadAccess(){
    setACLAccess(ME_ACL_READ_ACCSS, ME_ACL_PUBLIC_IDENTIFIER);
}

void MeACL::setPublicWriteAccess(){
    setACLAccess(ME_ACL_WRITE_ACCSS, ME_ACL_PUBLIC_IDENTIFIER);
}

#pragma 角色权限
void MeACL::setRoleReadAccess(const char* role){
    setACLRoleAccess(ME_ACL_READ_ACCSS, role);
}

void MeACL::setRoleWriteAccess(const char* role){
    setACLRoleAccess(ME_ACL_WRITE_ACCSS, role);
}

void MeACL::setRoleReadAccess(MeRole* role){
    setRoleReadAccess(role->stringValue(ME_ROLE_KEY));
}

void MeACL::setRoleWriteAccess(MeRole* role){
    setRoleWriteAccess(role->stringValue(ME_ROLE_KEY));
}

void MeACL::setACLRoleAccess(const char* name, const char* role, bool access){
    char *rolename = (char *)malloc(strlen(ME_ROLE_KEY) + strlen(role) + 2);
    strcpy(rolename, ME_ROLE_KEY);
    strcat(rolename, ":");
    strcat(rolename, role);
    setACLAccess(name, rolename);
}

#pragma 用户权限
void MeACL::setUserReadAccess(const char* userId){
    setACLAccess(ME_ACL_READ_ACCSS, userId);
}

void MeACL::setUserWriteAccess(const char* userId){
    setACLAccess(ME_ACL_WRITE_ACCSS, userId);
}

void MeACL::setUserReadAccess(MeUser* user){
    setUserReadAccess(user->objectId());
}

void MeACL::setUserWriteAccess(MeUser* user){
    setUserWriteAccess(user->objectId());
}

#pragma 通用接口
void MeACL::setACLAccess(const char* name, const char* identifier, bool access){
    JSONObject accessObject;
    accessObject.setDestruct(false);
    accessObject.put(name, access);
    if (!has(identifier)) {
        put(identifier, &accessObject);
    } else {
        accessObject = jsonValue(identifier);
        accessObject.put(name, access);
    }
}

MeACL::~MeACL(){
    cout << toString() << endl;
}
