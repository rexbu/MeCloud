//
//  MeIOSHttpDataCallback.h
//  MeCloud
//
//  Created by super on 2017/9/14.
//  Copyright © 2017年 Rex. All rights reserved.
//

#ifndef    __MEIOSHTTPDATACALLBACK_H_
#define    __MEIOSHTTPDATACALLBACK_H_

#include "MeHttpFileCallback.h"
#include "MeCallback.h"

class MeIOSHttpDataCallback: public MeHttpDataCallback{
public:
    MeIOSHttpDataCallback();
    ~MeIOSHttpDataCallback();
    
    virtual void done(MeObject* object, MeException* err, uint32_t size = 1);
    virtual void progress(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write);
    
    void setBlock(MeCallbackBlock block);
    void setProgressBlock(MeHttpFileProgressBlock block);
    
protected:
    MeCallbackBlock m_block;
    MeHttpFileProgressBlock m_progress_block;
};

#endif
