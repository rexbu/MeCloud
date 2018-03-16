/**
 * file :	MeRole.cpp
 * author :	Rex
 * create :	2017-07-18 17:58
 * func : 
 * history:
 */

#include "MeRole.h"

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
