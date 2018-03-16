//
//  MeDownloadFile.hpp
//  MeCloud
//
//  Created by super on 2017/7/31.
//  Copyright © 2017年 Rex. All rights reserved.
//

#ifndef MeDownloadFile_h
#define MeDownloadFile_h

#include "MeFile.h"

using namespace mc;
class MeDownloadFile: public MeFile {
public:
    
    MeDownloadFile(JSONObject* obj = NULL, const char* className = "File");
    MeDownloadFile(MeDownloadFile* file);
    ~MeDownloadFile();
    
    void setDownloadUrl(const char *url);
    
    /// 下载
    void download(HttpFileCallback* callback);
    
#ifdef __IOS__
    void download(MeHttpFileCallbackBlock block, MeHttpFileProgressBlock progressBlock);
#endif
};

#endif /* MeDownloadFile_h */
