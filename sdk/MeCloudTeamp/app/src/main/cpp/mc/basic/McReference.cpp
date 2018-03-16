/**
 * file :	McReference.cpp
 * author :	Rex
 * create :	2017-07-07 15:48
 * func : 
 * history:
 */

#include "McReference.h"

using namespace mc;

ReferenceCache* ReferenceCache::m_instance = NULL;
ReferenceCache* ReferenceCache::shareInstance(){
    if (m_instance == NULL) {
        m_instance = new ReferenceCache();
    }
    return m_instance;
}

void ReferenceCache::destroyInstance(){
    if (m_instance != NULL){
        delete m_instance;
        m_instance = NULL;
    }
}

ReferenceCache::~ReferenceCache(){
    for (int i=0; i<m_cache.size(); i++) {
        delete m_cache[i];
    }
    
    m_cache.clear();
}

Reference* ReferenceCache::get(){
    for (int i=0; i<m_cache.size(); i++) {
        if (m_cache[i]->idle()) {
            return m_cache[i];
        }
    }
    
    return NULL;
}
