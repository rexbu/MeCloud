
//
//  MeIOSHttpFileCallBack.cpp
//  MeCloud
//
//  Created by super on 2017/7/25.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "MeIOSHttpFileCallback.h"
#include "MeIOSCallback.h"
#include "MeUploadFile.h"
#include "MeDownloadFile.h"

MeIOSHttpFileCallback::MeIOSHttpFileCallback(const char* classname, MeFile* file):MeHttpFileCallback(classname, file) {
}

void MeIOSHttpFileCallback::progress(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write) {
    dispatch_sync(dispatch_get_main_queue(), ^(){
        m_progress_block(writen, total_writen, total_expect_write);
    });
}

void MeIOSHttpFileCallback::done(MeFile* file, MeException* err, uint32_t size) {
    if (file->fileType() == ME_HTTPFILE_UPLOAD) {
        MeBlockCallback *callback = new MeBlockCallback(file->className(), file);
        if (err) {
            dispatch_sync(dispatch_get_main_queue(), ^(){
                m_block(file, err, size);
            });
        } else {
            MeCallbackBlock block = ^(MeObject *obj, MeException *err, uint32_t size) {
                m_block(file, err, size);
                delete this;
            };
            callback->setBlock(block);
            MeUploadFile *uploadFile = (MeUploadFile *)file;
            uploadFile->uploadFileInfomation(callback);
        }
    } else {
        dispatch_sync(dispatch_get_main_queue(), ^(){
            m_block(file, err, size);
            delete this;
        });
    }
}

void MeIOSHttpFileCallback::setBlock(MeHttpFileCallbackBlock block) {
    m_block = (__bridge MeHttpFileCallbackBlock)Block_copy((__bridge void *)block);
}

void MeIOSHttpFileCallback::setProgressBlock(MeHttpFileProgressBlock block) {
    m_progress_block = (__bridge MeHttpFileProgressBlock)Block_copy((__bridge void *)block);
}

MeIOSHttpFileCallback::~MeIOSHttpFileCallback() {
    Block_release(m_block);
    Block_release(m_progress_block);
}
