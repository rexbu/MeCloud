//
//  MeDownloadFile.m
//  MeCloud
//
//  Created by super on 2017/7/31.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MeDownloadFile.h"
#import "MeDownloadFile.h"

void MeDownloadFile::download(MeHttpFileCallbackBlock block, MeHttpFileProgressBlock progressBlock) {
    MeIOSHttpFileCallback *callback = new MeIOSHttpFileCallback(m_classname, this);
    callback->setBlock(block);
    callback->setProgressBlock(progressBlock);
    
    MeDownloadFile::download(callback);
}
