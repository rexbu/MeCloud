/**
 * file :	MeCallback.cpp
 * author :	bushaofeng
 * create :	2016-08-27 23:28
 * func : 
 * history:
 */

#include "bs.h"
#include "MeCallback.h"
#include "MeObject.h"
#include "MeCloud.h"
#include "ThreadPool.h"

#pragma --mark "对象回调"
void MeCallback::done(int http_code, status_t st, char* text){
//    if (http_code==CURLE_OPERATION_TIMEDOUT || http_code== CURLE_COULDNT_RESOLVE_HOST) {
//        return;
//    }
    
    // 连接错误, VisioninSDK应该在外部判断st=BS_CONNERR、BS_SENDERR
    if (http_code!=HTTP_OK || st!=BS_SUCCESS)
    {
        MeException e;
        e.put("httpCode", http_code);
        e.put("errCode", st);
        e.put("errMsg", text);
        done(NULL, &e);
        return;
    }
    
    char* c = text;
    while ((*c==' ' || *c=='\n')&&(c-text)<strlen(text)) {
        c++;
    }
    
    if (*c=='{') {
        MeException obj(c);
        if (obj.has("errCode")) {
            done(NULL, &obj);
        }
        else{
            m_object->copySelf(&obj);
            done(m_object, NULL);
        }
    }
    else if(*c=='['){
        JSONArray array(c);
        uint32_t size = array.size();
        if (size==0) {
            MeException e;
            e.put("errCode", BS_NOTFOUND);
            e.put("errMsg", "Not Found");
            done(NULL, &e);
            return;
        }
        MeObject* objs = new MeObject[size];
        for (int i=0; i<size; i++) {
            JSONObject d = array.jsonValue(i);
            // d.CopyFrom(array[i], d.GetAllocator());
            MeObject* o = &objs[i];
            o->setClassName(m_classname);
            o->copySelf(&d, false);
        }
        done(objs, NULL, size);
        delete[] objs;
    }
    else{
        MeException e;
        e.put("errCode", BS_JSONPARSE);
        e.put("errMsg", "JSON Parse Error");
        done(NULL, &e);
        return;
    }
}

