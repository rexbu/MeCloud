//
//  MeDownloadFile.cpp
//  MeCloud
//
//  Created by super on 2017/7/31.
//  Copyright © 2017年 Rex. All rights reserved.
//

#include "MeDownloadFile.h"

MeDownloadFile::MeDownloadFile(JSONObject* obj, const char* className): MeFile(ME_HTTPFILE_DOWNLOAD, className, obj) {
    
}

MeDownloadFile::MeDownloadFile(MeDownloadFile* file): MeFile(file) {
    
}

void MeDownloadFile::setDownloadUrl(const char *url) {
    memset(m_url, 0, sizeof(m_url));
    if (strstr(url, "http://") || strstr(url, "https://")) {
        snprintf(m_url, sizeof(m_url), "%s", url);
    } else {
        // Url
        memset(m_url, 0, sizeof(m_url));
        if (has("type") && strlen(stringValue("type"))>0) {
            snprintf(m_url, sizeof(m_url), "http://wyh1111.oss-cn-hangzhou.aliyuncs.com/%s.%s", url, stringValue("type"));
        } else{
            snprintf(m_url, sizeof(m_url), "http://wyh1111.oss-cn-hangzhou.aliyuncs.com/%s", url);
        }
    }
}

MeDownloadFile::~MeDownloadFile() {
}

/// 下载
void MeDownloadFile::download(HttpFileCallback* callback) {
    MeCloud::shareInstance()->download(m_url, filePath(),callback);
}
