//
//  MeUploadFile.m
//  MeCloud
//
//  Created by super on 2017/7/31.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeUploadFile.h"
#import "MeIOSCallback.h"
#import <Foundation/Foundation.h>

void MeUploadFile::upload(MeHttpFileCallbackBlock block, MeHttpFileProgressBlock progressBlock) {
    getAuthInfomation(^(MeObject *obj, MeException *err, uint32_t size) {
        if (err || obj->intValue("errCode") != 0) {
            block(this, err, size);
        } else {
            MeIOSHttpFileCallback *callback = new MeIOSHttpFileCallback(m_classname, this);
            callback->setBlock(block);
            callback->setProgressBlock(progressBlock);
            upload(callback, obj);
        }
    });
}

// 获取上传文件的授权消息
void MeUploadFile::getAuthInfomation(MeCallbackBlock callbackBlock){
    MeBlockCallback* callback = new MeBlockCallback(m_classname, this);
    callback->setBlock(callbackBlock);
    MeUploadFile::getAuthInfomation(callback);
}
