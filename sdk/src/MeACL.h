/**
 * file :	MeACL.h
 * author :	Rex
 * create :	2017-07-18 17:57
 * func : 
 * history:
 */

#ifndef	__MEACL_H_
#define	__MEACL_H_

#include <map>
#include "JSON.h"

class MeRole;
class MeUser;
#define ME_ACL_KEY "acl"

using namespace mc;
using namespace std;
class MeACL: public JSONObject{
public:
    MeACL();
    MeACL(MeACL* acl);
    ~MeACL();
    
    MeACL& operator=(const MeACL &acl);
    
    void setPublicReadAccess();
    void setPublicWriteAccess();
    
    void setRoleReadAccess(const char* role);
    void setRoleWriteAccess(const char* role);
    void setRoleDeleteAccess(const char* role);
    void setRoleReadAccess(MeRole* role);
    void setRoleWriteAccess(MeRole* role);
    void setRoleDeleteAccess(MeRole* role);
    
    void setUserReadAccess(const char* userId);
    void setUserWriteAccess(const char* userId);
    void setUserDeleteAccess(const char* userId);
    void setUserReadAccess(MeUser* user);
    void setUserWriteAccess(MeUser* user);
    void setUserDeleteAccess(MeUser* user);
    
protected:
    map<string, JSONObject*>        m_object_map;
    
private:
    void setACLAccess(const char* name, const char* identifier, bool access=true);
    void setACLRoleAccess(const char* name, const char* role, bool access=true);
    void clear();
};
#endif
