/**
 * file :	MeObject.mm
 * author :	Rex
 * create :	2017-07-06 20:22
 * func : 
 * history:
 */

#include "McBasic.h"
#include "MeObject.h"
#include "MeIOSCallback.h"

void MeObject::save(MeCallbackBlock block){
    MeBlockCallback* callback = new MeBlockCallback(m_classname, this);
    callback->setBlock(block);
    MeObject::save(callback);
}
