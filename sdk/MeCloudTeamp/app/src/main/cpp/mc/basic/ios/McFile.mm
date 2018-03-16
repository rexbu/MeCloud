/**
 * file :	McFile.mm
 * author :	bushaofeng
 * create :	2016-08-27 11:38
 * func : 
 * history:
 */

#import <Foundation/Foundation.h>
#include <stdio.h>
#include <string.h>
#include "McFile.h"
using namespace mc;

//const char* FileManager::m_root_path = [[NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0] UTF8String];
//const char* FileManager::m_resource_path = [[NSBundle mainBundle].resourcePath UTF8String];

// 设置成全局变量发现初始化会有问题
const char* FileManager::resourcePath(){
    return [[NSBundle mainBundle].resourcePath UTF8String];
}

static char g_root_path[1024] = {0};
const char* FileManager::rootPath(){
    if (strlen(g_root_path)==0) {
        const char* p = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0].UTF8String;
        strcpy(g_root_path, p);
    }
    return g_root_path;
}
