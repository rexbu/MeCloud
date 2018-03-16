//
//  MeHttpFileCallback.h
//  MeCloud
//
//  Created by super on 2017/7/24.
//  Copyright © 2017年 Rex. All rights reserved.
//

#ifndef __MEDOWNLOADCALLBACK_H_
#define __MEDOWNLOADCALLBACK_H_

#include "McBasic.h"
using namespace mc;

class MeFile;
class MeException;
class MeObject;

#ifdef __IOS__
typedef void (^MeHttpFileCallbackBlock)(MeFile *obj, MeException *err, uint32_t size);
typedef void (^MeHttpFileProgressBlock)(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write);
#else
typedef void (*MeHttpFileCallback_func)(MeFile *obj, MeException *err, uint32_t size);
typedef void (*MeHttpFileProgress_func)(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write);
#endif

class MeHttpFileCallback: public HttpFileCallback, public Reference{
public:
    MeHttpFileCallback(const char* classname=NULL, MeFile* file=NULL){
        m_classname = classname;
        m_file = file;
    }
    
    virtual void done(int http_code, status_t st, const char* text);
    virtual void done(MeFile* file, MeException* err, uint32_t size = 1) = 0;
    
    const char* m_classname;
    MeFile*   m_file;
};

class MeHttpDataCallback: public HttpFileCallback, public Reference{
public:
    MeHttpDataCallback(){}
    
    virtual void done(int http_code, status_t st, const char* text);
    virtual void done(MeObject* json, MeException* err, uint32_t size = 1) = 0;
};

#endif /* MeHttpFileCallback.h */
