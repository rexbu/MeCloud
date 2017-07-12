/**
 * file :	MeFile.cpp
 * author :	Rex
 * create :	2016-09-20 23:46
 * func : 
 * history:
 */

#include "MeFile.h"
#include "McFile.h"

static char g_root_path[1024];
const char* MeFile::m_root_path = NULL;

MeFile::MeFile(JSONObject* obj):
MeObject("MeFile", obj){
    init();
}

MeFile::MeFile(MeFile* file):
MeObject(file){
    init();
}

void MeFile::init(){
    // 文件名
    m_name = stringValue("name");
    snprintf(m_path, sizeof(m_path), "%s/%s", rootPath(), m_name);
    
    // Url
    memset(m_url, 0, sizeof(m_url));
    if (m_objectid!=NULL) {
        if (has("type") && strlen(stringValue("type"))>0) {
            snprintf(m_url, sizeof(m_url), "http://file.visualogies.com/%s.%s", m_objectid, stringValue("type"));
        }
        else{
            snprintf(m_url, sizeof(m_url), "http://file.visualogies.com/%s", m_objectid);
        }
    }
    
    // 解压文件名
    memset(m_unzip_name, 0, sizeof(m_unzip_name));
    memset(m_unzip_path, 0, sizeof(m_unzip_path));
    if (memcmp(m_name+strlen(m_name)-4, ".zip", 4)==0) {
        memcpy(m_unzip_name, m_name, strlen(m_name)-4);
        snprintf(m_unzip_path, sizeof(m_unzip_path), "%s/%s", rootPath(), m_unzip_name);
    }
}

const char* MeFile::getUrl(){
    if (strlen(m_url)==0) {
        return NULL;
    }
    
    return m_url;
}

/// 下载
void MeFile::download(DownCallback* callback){
    MeCloud::shareInstance()->download(m_url, filePath(),callback);
}
/// 返回存储路径
const char* MeFile::filePath(){
    return m_path;
}
/// 解压
bool MeFile::unzip(){
    return true;
    // return Zip::unzip(filePath(), rootPath(), callback);
}

/// 解压后文件名，如果不是zip文件返回NULL
const char* MeFile::unzipFileName(){
    if (strlen(m_unzip_name)==0) {
        return NULL;
    }
    
    return m_unzip_name;
}
/// 解压后文件路径，如果不是zip文件返回nil
const char* MeFile::unzipFilePath(){
    if (strlen(m_unzip_path)==0) {
        return NULL;
    }
    
    return m_unzip_path;
}

/// 根目录
const char* MeFile::rootPath(){
    if (m_root_path==NULL) {
        snprintf(g_root_path, sizeof(g_root_path), "%s/__me", FileManager::rootPath());
        // 如果不存在则创建文件夹
        if (!FileManager::exist(g_root_path)) {
            FileManager::mkdir(g_root_path);
        }
        m_root_path = g_root_path;
    }
    
    return m_root_path;
}
