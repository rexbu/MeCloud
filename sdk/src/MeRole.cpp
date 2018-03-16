/**
 * file :	MeRole.cpp
 * author :	Rex
 * create :	2017-07-18 17:58
 * func : 
 * history:
 * note: 一条角色数据只能对应一个user，如果设置了多个，最后设置的一个会覆盖之前的
 */

#include "MeRole.h"

MeRole::MeRole(MeObject *object):MeObject(object) {
}

MeRole::MeRole(const char* rolename):
MeObject("Role"){
    put(ME_ROLE_KEY, rolename);
}

void MeRole::setUser(MeUser* user){
    setUser(user->objectId());
}
void MeRole::setUser(const char* userId){
    put("user", userId);
}
