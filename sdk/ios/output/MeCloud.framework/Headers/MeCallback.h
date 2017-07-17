/**
 * file :	MeCallback.h
 * author :	bushaofeng
 * create :	2016-08-27 23:28
 * func : 
 * history:
 */

#ifndef	__MECALLBACK_H_
#define	__MECALLBACK_H_

#include "McBasic.h"
using namespace mc;

class MeObject;
class MeException;

#ifdef __IOS__
typedef void (^MeCallbackBlock)(MeObject* obj, MeException* err, uint32_t size);
#else
typedef void (*MeCallback_func)(MeObject* obj, MeException* err, uint32_t size);
#endif

class MeCallback: public HttpCallback, public Reference{
public:
    MeCallback(const char* classname=NULL, MeObject* obj=NULL){
        m_classname = classname;
        m_object = obj;
    }
    
    virtual void done(int http_code, status_t st, char* text);
    virtual void done(MeObject* obj, MeException* err, uint32_t size = 1) = 0;
    
    const char* m_classname;
    MeObject*   m_object;
};

#endif
