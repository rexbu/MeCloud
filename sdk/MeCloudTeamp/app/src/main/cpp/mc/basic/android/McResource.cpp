/**
 * file :	McResource.cpp
 * author :	Rex
 * create :	2016-12-13 15:00
 * func : 
 * history:
 */

#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "McFile.h"
 
extern JavaVM*  g_jvm;

using namespace mc;
AAssetManager* Resource::m_asset_manager=NULL;

Resource::Resource(jobject assetManager){
    m_exist = false;
    m_bytes = NULL;
    m_bytes_size = 0;
    if (m_asset_manager==NULL)
    {
        JNIEnv* env;
        g_jvm->AttachCurrentThread(&env, NULL);
        m_asset_manager = AAssetManager_fromJava(env, assetManager);
    }
}

Resource::Resource(){
    m_exist = false;
    m_bytes = NULL;
    m_bytes_size = 0;
}

int Resource::read(const char* file){
    if (m_asset_manager==NULL){
        return -2;
    }

    AAsset* asset = AAssetManager_open(m_asset_manager, file,AASSET_MODE_UNKNOWN);
    if (asset==NULL)
    {
        return -1;
    }

    /*获取文件大小*/ 
    m_bytes_size = AAsset_getLength(asset); 
    if (m_bytes!=NULL)
    {
        free(m_bytes);
    }
    m_bytes = (uint8_t*)malloc(m_bytes_size); 
     
    int num = AAsset_read(asset, m_bytes, m_bytes_size); 
    AAsset_close(asset); 
    return num;
}

Resource::~Resource(){
    if (m_bytes != NULL)
    {
        free(m_bytes);
        m_bytes = NULL;
    }
}
