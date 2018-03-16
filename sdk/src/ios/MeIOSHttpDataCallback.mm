//
//  MeIOSHttpDataCallback.mm
//  MeCloud
//
//  Created by super on 2017/9/14.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "MeIOSHttpDataCallback.h"
#import <Foundation/Foundation.h>

MeIOSHttpDataCallback::MeIOSHttpDataCallback(): MeHttpDataCallback() {
    
}

void MeIOSHttpDataCallback::progress(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write) {
    dispatch_sync(dispatch_get_main_queue(), ^(){
        m_progress_block(writen, total_writen, total_expect_write);
    });
}

void MeIOSHttpDataCallback::done(MeObject* object, MeException* err, uint32_t size) {
    dispatch_sync(dispatch_get_main_queue(), ^(){
        m_block(object, err, size);
        delete this;
    });
}

void MeIOSHttpDataCallback::setBlock(MeCallbackBlock block) {
    m_block = (__bridge MeCallbackBlock)Block_copy((__bridge void *)block);
}

void MeIOSHttpDataCallback::setProgressBlock(MeHttpFileProgressBlock block) {
    m_progress_block = (__bridge MeHttpFileProgressBlock)Block_copy((__bridge void *)block);
}

MeIOSHttpDataCallback::~MeIOSHttpDataCallback() {
    Block_release(m_block);
    Block_release(m_progress_block);
}
