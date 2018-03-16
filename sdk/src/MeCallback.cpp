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
void MeCallback::done(int http_code, status_t st, char* text) {    
    char *decrypt = NULL;
    if (text && http_code == HTTP_OK) {
        decrypt = MeCloud::shareInstance()->decrypt(text, (int)strlen(text));
    } else {
        // 异常处理，防止崩溃
        const char* error = "服务器异常";
        decrypt = (char *)malloc(strlen(error) + 1);
        memcpy(decrypt, error, strlen(error));
    }
    
    if (http_code != HTTP_OK) {
        MeException e;
        e.put("httpCode", http_code);
        e.put("errMsg", text);
        done(NULL, &e);
        free(decrypt);
        return;
    }
    
    char* c = decrypt;
    while ((*c==' ' || *c=='\n')&&(c-decrypt)<strlen(decrypt)) {
        c++;
    }
    
    if (*c=='{') {
        JSONObject obj(c);
        if (m_object) {
            m_object->copySelf(&obj);
            done(m_object, NULL);
        } else {
            MeObject *object = new MeObject();
            object->copySelf(&obj, false);
            done(object, NULL);
            delete object;
        }
    } else if (*c=='[') {
        JSONArray array(c);
        uint32_t size = array.size();
        MeObject* objs = new MeObject[size];
        for (int i=0; i < size; i++) {
            JSONObject d = array.jsonValue(i);
            MeObject* o = &objs[i];
            o->setClassName(m_classname);
            o->copySelf(&d, false);
        }
        done(objs, NULL, size);
        delete[] objs;
    } else{
        MeException e;
        e.put("errCode", BS_JSONPARSE);
        e.put("errMsg", "JSON Parse Error");
        e.put("info", c);
        done(NULL, &e);
    }
  
    free(decrypt);
}
