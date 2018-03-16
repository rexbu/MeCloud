/**
 * file :	MeIOSCallback.mm
 * author :	Rex
 * create :	2017-07-06 20:39
 * func : 
 * history:
 */

#include "MeIOSCallback.h"

MeBlockCallback::MeBlockCallback(MeCallbackBlock block, const char* classname, MeObject* obj):MeCallback(classname, obj){
    setBlock(block);
}

MeBlockCallback::MeBlockCallback(const char* classname, MeObject* obj):
MeCallback(classname, obj){
}

void MeBlockCallback::done(MeObject* obj, MeException* err, uint32_t size){
    dispatch_sync(dispatch_get_main_queue(), ^(){
        m_block(obj, err, size);
    });
}

void MeBlockCallback::setBlock(MeCallbackBlock block){
    m_block = (__bridge MeCallbackBlock)Block_copy((__bridge void *)block);
}

MeBlockCallback::~MeBlockCallback(){
    Block_release(m_block);
}
