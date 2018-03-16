//
//  MeUploadFile.cpp
//  MeCloud
//
//  Created by super on 2017/7/31.
//  Copyright © 2017年 Rex. All rights reserved.
//

#include "MeUploadFile.h"
#include "McHmacAuth.h"
#include "MeUser.h"

MeUploadFile::MeUploadFile(const char* className, JSONObject* obj): MeFile(ME_HTTPFILE_UPLOAD, className, obj) {
    init();
}

MeUploadFile::MeUploadFile(MeUploadFile* file): MeFile(file) {
    init();
}

MeUploadFile::~MeUploadFile() {
}

void MeUploadFile::init() {
    MeFile::init();
}

void MeUploadFile::setUploadFilePath(const char* path) {
    snprintf(m_upload_path, sizeof(m_upload_path), "%s", path);
}

void MeUploadFile::setUploadData(uint8_t *data, int size) {
    if (m_data) {
        delete m_data;
    }
    
    m_data = new uint8_t[size];
    memcpy(m_data, data, size);
    m_content_length = size;
}

void MeUploadFile::upload(HttpFileCallback* callback, JSONObject* object) {
    const char* access_key_secret = object->stringValue("access_key_secret");
    const char* access_key_id = object->stringValue("access_key_id");
    const char* security_token = object->stringValue("security_token");
    
    time_t timep;
    time(&timep);
    tm* time = gmtime(&timep);
    char date[80];
    strftime(date, 80, "%a, %d %b %Y %T GMT", time);
    const char *content_type = "multipart/form-data";
    
    char resource[1024];
    snprintf(resource, sizeof(resource), "PUT\n\n%s\n%s\nx-oss-security-token:%s\n/%s/%s", content_type, date, security_token, object->stringValue("bucket"), m_objectid);
    
    char key_id[1024];
    snprintf(key_id, sizeof(key_id), "%s", access_key_id);
    
    char key_secret[1024];
    snprintf(key_secret, sizeof(key_secret), "%s", access_key_secret);
    
    char output[MC_AUTH_OUPUT_SIZE];
    mc::mc_auth(resource, key_id, key_secret, output);
    
    // 签名计算
    vector<pair<string, string> > authHeader;
    // 获取文件的大小
    
    authHeader.push_back(pair<string, string>("Authorization", output));
    authHeader.push_back(pair<string, string>("Content-Type", content_type));
    authHeader.push_back(pair<string, string>("Date", date));
    authHeader.push_back(pair<string, string>("x-oss-security-token", security_token));
    
    resetMeUrl(object);
    char contentString[1024];
    
    if (m_data) {
        snprintf(contentString, sizeof(contentString), "%llu", m_content_length);
        authHeader.push_back(pair<string, string>("Content-Length", contentString));
        MeCloud::shareInstance()->upload(m_url, m_data, (int)m_content_length, authHeader, callback);
    } else {
        FILE* file = fopen(m_upload_path, "rb");
        fseek(file , 0 , SEEK_END);
        m_content_length = ftell (file);
        rewind (file);
        snprintf(contentString, sizeof(contentString), "%llu", m_content_length);
        authHeader.push_back(pair<string, string>("Content-Length", contentString));
        MeCloud::shareInstance()->upload(m_url, m_upload_path, authHeader, callback);
    }
}

void MeUploadFile::resetMeUrl(JSONObject* object) {
    const char *reginId = object->stringValue("region_id");
    me_bucket = object->stringValue("bucket");
    memset(m_url, 0, sizeof(m_url));
    if (has("type") && strlen(stringValue("type")) > 0) {
        snprintf(m_url, sizeof(m_url), "http://%s.%s.aliyuncs.com/%s.%s", me_bucket, reginId, m_objectid, stringValue("type"));
    } else{
        snprintf(m_url, sizeof(m_url), "http://%s.%s.aliyuncs.com/%s", me_bucket, reginId, m_objectid);
    }
}

void MeUploadFile::getAuthInfomation(MeCallback *callback) {
    memset(m_url, 0, sizeof(m_url));
    snprintf(m_url, sizeof(m_url), "%stoken", MeCloud::shareInstance()->fileUrl());
    MeCloud::shareInstance()->get(m_url, callback);
}

void MeUploadFile::uploadFileInfomation(MeCallback *callback) {
    memset(m_url, 0, sizeof(m_url));
    snprintf(m_url, sizeof(m_url), "%supload", MeCloud::shareInstance()->fileUrl());
    JSONObject *object = new JSONObject();
    object->put("_sid", m_objectid);
    object->put("name", m_objectid);
    object->put("type", "jpg"); //目前写死
    object->put("platform", me_platform);
    object->put("bucket", me_bucket);
    object->put("size", (int)m_content_length);
    MeUser *user = MeUser::currentUser();
    if (user) {
        object->put("user", user->objectId());
    }
    
    MeCloud::shareInstance()->post(m_url, object->toString(), callback);
    delete object;
}
