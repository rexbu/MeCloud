/**
 * file :	MeRole.h
 * author :	Rex
 * create :	2017-07-18 17:57
 * func : 
 * history:
 */

#ifndef	__MEROLE_H_
#define	__MEROLE_H_

#include "MeObject.h"
#include "MeUser.h"
#include "MeQuery.h"

#define ME_ROLE_KEY "role"

class MeRole: public MeObject{
public:
    MeRole(const char* rolename);
    
    void setUser(MeUser* user);
    void setUser(const char* userId);
};

#endif
