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
    ReferenceCache* cache = ReferenceCache::shareInstance();
    MeBlockCallback* callback = (MeBlockCallback*)cache->get();
    if (callback==NULL) {
        callback = new MeBlockCallback(m_classname, this);
        cache->add(callback);
    }
    callback->setBlock(block);
    
    MeObject::save(callback);
}
