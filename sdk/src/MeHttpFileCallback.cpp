//
//  MeDownloadCallback.cpp
//  MeCloud
//
//  Created by super on 2017/7/24.
//  Copyright © 2017年 Rex. All rights reserved.
//

#include "MeHttpFileCallback.h"
#include "MeFile.h"
#include "bs.h"
#include "string.h"

void MeHttpFileCallback::done(int http_code, status_t st, const char* text){
    if (http_code != HTTP_OK) {
        MeException e;
        e.put("httpCode", http_code);
        e.put("errMsg", text);
        e.put("info", text);
        done(m_file, &e);
        return;
    }
    
    done(m_file, NULL);
}

void MeHttpDataCallback::done(int http_code, status_t st, const char *text) {
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
        e.put("info", text);
        done(NULL, &e);
        free(decrypt);
        return;
    }
    
    char* c = (char *)decrypt;
    while ((*c==' ' || *c=='\n')&&(c-text)<strlen(text)) {
        c++;
    }
    
    if (*c=='{') {
        JSONObject obj(c);
        MeObject *object = new MeObject();
        object->copySelf(&obj, false);
        done(object, NULL);
        delete object;
    } else if(*c=='[') {
        JSONArray array(c);
        uint32_t size = array.size();
        MeObject* objs = new MeObject[size];
        done(objs, NULL, size);
        delete[] objs;
    } else {
        MeException e;
        e.put("errCode", BS_JSONPARSE);
        e.put("errMsg", "JSON Parse Error");
        e.put("info", c);
        done(NULL, &e);
    }
    
    free(decrypt);
}
