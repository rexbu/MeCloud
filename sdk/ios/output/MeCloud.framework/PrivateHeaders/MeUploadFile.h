//
//  MeUploadFile.h
//  MeCloud
//
//  Created by super on 2017/7/31.
//  Copyright © 2017年 Rex. All rights reserved.
//

#ifndef MeUploadFile_h
#define MeUploadFile_h

#include "MeFile.h"

using namespace mc;
class MeUploadFile: public MeFile {
public:
    MeUploadFile(const char* className = "File", JSONObject* obj = NULL);
    MeUploadFile(MeUploadFile* file);
    ~MeUploadFile();
    
    void init();
    
    /// 上传
    void upload(HttpFileCallback* callback, JSONObject* object);
    /// 获取上传授权的相关http header信息
    void getAuthInfomation(MeCallback *callback);
    /// 上传文件的本地路径
    void setUploadFilePath(const char* path);
    /// 上传图片二进制数据
    void setUploadData(uint8_t *data, int size);
    /// 上传文件成功后给发送反馈信息给服务器
    void uploadFileInfomation(MeCallback *callback);
    /// 返回处理文件类型（目前有上传和下载）
    
#ifdef __IOS__
    void upload(MeHttpFileCallbackBlock block, MeHttpFileProgressBlock progressBlock);
#endif
    
private:
    void resetMeUrl(JSONObject* object);
#ifdef __IOS__
    void getAuthInfomation(MeCallbackBlock callbackBlock);
#endif
    
protected:
    char m_upload_path[256] = "\0";
    uint8_t* m_data = NULL;
    uint64_t m_content_length;
};

#endif /* MeUploadFile_h */
