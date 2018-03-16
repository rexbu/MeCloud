/**
 * file :	MeIOSCallback.h
 * author :	Rex
 * create :	2017-07-06 20:39
 * func : 
 * history:
 */

#ifndef	__MEIOSCALLBACK_H_
#define	__MEIOSCALLBACK_H_

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#include "MeCallback.h"

class MeBlockCallback: public MeCallback{
public:
    MeBlockCallback(MeCallbackBlock block, const char* classname=NULL, MeObject* obj=NULL);
    MeBlockCallback(const char* classname=NULL, MeObject* obj=NULL);
    ~MeBlockCallback();
    
    virtual void done(MeObject* obj, MeException* err, uint32_t size = 1);
    
    void setBlock(MeCallbackBlock block);
protected:
    MeCallbackBlock m_block;
};

#endif
