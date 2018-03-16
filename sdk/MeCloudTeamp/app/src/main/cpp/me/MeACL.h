/**
 * file :	MeACL.h
 * author :	Rex
 * create :	2017-07-18 17:57
 * func : 
 * history:
 */

#ifndef	__MEACL_H_
#define	__MEACL_H_

#include "MeObject.h"
#include "MeUser.h"
#include "MeRole.h"

#define ME_ACL_KEY "acl"

class MeACL: public JSONObject{
public:
    ~MeACL();
    
    void setPublicReadAccess();
    void setPublicWriteAccess();
    
    void setRoleReadAccess(const char* role);
    void setRoleWriteAccess(const char* role);
    void setRoleReadAccess(MeRole* role);
    void setRoleWriteAccess(MeRole* role);
    
    void setUserReadAccess(const char* userId);
    void setUserWriteAccess(const char* userId);
    void setUserReadAccess(MeUser* user);
    void setUserWriteAccess(MeUser* user);
    
private:
    void setACLAccess(const char* name, const char* identifier, bool access=true);
    void setACLRoleAccess(const char* name, const char* role, bool access=true);
};
#endif
