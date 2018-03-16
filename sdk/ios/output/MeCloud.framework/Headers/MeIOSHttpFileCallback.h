//
//  MeIOSHttpFileCallback.h
//  MeCloud
//
//  Created by super on 2017/7/25.
//  Copyright © 2017年 Rex. All rights reserved.
//

#ifndef	__MEIOSHTTPFILECALLBACK_H_
#define	__MEIOSHTTPFILECALLBACK_H_

#include "MeHttpFileCallback.h"
#include "MeCallback.h"

class MeIOSHttpFileCallback: public MeHttpFileCallback{
public:
    MeIOSHttpFileCallback(const char *classname=NULL, MeFile *file=NULL);
    ~MeIOSHttpFileCallback();
    
    virtual void done(MeFile *file, MeException *err, uint32_t size = 1);
    virtual void progress(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write);
    
    void setBlock(MeHttpFileCallbackBlock block);
    void setProgressBlock(MeHttpFileProgressBlock block);
    
protected:
    MeHttpFileCallbackBlock m_block;
    MeHttpFileProgressBlock m_progress_block;
    void uploadFileInfomation(MeCallbackBlock block);
};

#endif
