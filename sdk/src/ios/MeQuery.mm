//
//  MeQuery.m
//  MeCloud
//
//  Created by super on 2017/7/20.
//  Copyright © 2017年 Rex. All rights reserved.
//

#include "MeQuery.h"
#include "MeIOSCallback.h"

void MeQuery::find(MeCallbackBlock block){
    MeBlockCallback* callback = new MeBlockCallback(m_classname);
    callback->setBlock(block);
    MeQuery::find(callback);
}

void MeQuery::get(const char* objectId, MeCallbackBlock block){
    MeBlockCallback* callback = new MeBlockCallback(m_classname);
    callback->setBlock(block);
    MeQuery::get(objectId, callback);
}
