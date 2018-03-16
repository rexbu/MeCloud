/**
 * file :	MeFile.cpp
 * author :	Rex
 * create :	2016-09-20 23:46
 * func : 
 * history:
 */

#include "MeFile.h"
#include <time.h>

static char g_root_path[1024];
const char* MeFile::m_root_path = NULL;

MeFile::MeFile(me_file_type type, const char* className, JSONObject* obj): MeObject(className, obj){
    me_type = type;
}

MeFile::MeFile(MeFile* file):
MeObject(file){
}

MeFile::MeFile(MeObject *meObject, bool auth): MeObject(meObject, auth) {
    me_type = ME_HTTPFILE_Default;
}

MeFile::MeFile(const char* objectId, const char* className): MeObject(objectId, className) {
  me_type = ME_HTTPFILE_Default;
}

MeFile::~MeFile() {
}

void MeFile::init(){
    // 文件名
//    根据业务去求或者是下载文件类型判断下载完是不是需要解压
//    memset(m_unzip_name, 0, sizeof(m_unzip_name));
//    memset(m_unzip_path, 0, sizeof(m_unzip_path));
//    if (memcmp(m_name + strlen(m_name) - 4, ".zip", 4) == 0) {
//        memcpy(m_unzip_name, m_name, strlen(m_name) - 4);
//        snprintf(m_unzip_path, sizeof(m_unzip_path), "%s/%s", rootPath(), m_unzip_name);
//    }
}

void MeFile::setFilename(const char* filename) {
    snprintf(m_name, sizeof(m_name), "%s", filename);
    snprintf(m_path, sizeof(m_path), "%s/%s", rootPath(), m_name);
}

const char* MeFile::getUrl(){
    if (strlen(m_url)==0) {
        return NULL;
    }
    
    return m_url;
}

me_file_type MeFile::fileType() {
    return me_type;
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
//const char* MeFile::unzipFileName(){
//    if (strlen(m_unzip_name)==0) {
//        return NULL;
//    }
//
//    return m_unzip_name;
//}
/// 解压后文件路径，如果不是zip文件返回nil
//const char* MeFile::unzipFilePath(){
//    if (strlen(m_unzip_path)==0) {
//        return NULL;
//    }
//    
//    return m_unzip_path;
//}

/// 根目录
const char* MeFile::rootPath() {
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

const char* MeFile::imageUrl(int width, int height) {
    memset(m_image_url, 0, URL_SIZE);
    if (strcmp("oss", me_platform) == 0) {
        if (width > 0 && height > 0) {
            Crypt crypt;
            snprintf(m_image_url, URL_SIZE, "x-oss-process=image/resize,w_%d,h_%d", width, height);
            char *cryptoParam = MeCloud::shareInstance()->crypto(m_image_url, (int)strlen(m_image_url));
            memset(m_image_url, 0, URL_SIZE);
            snprintf(m_image_url, URL_SIZE, "%sfile/download/%s?%s", MeCloud::shareInstance()->restUrl(), objectId(), cryptoParam);
            free(cryptoParam);
        } else {
            snprintf(m_image_url, URL_SIZE, "%sfile/download/%s", MeCloud::shareInstance()->restUrl(), objectId());
        }
    } else {
        memcpy(m_image_url, stringValue("url"), URL_SIZE);
    }
    
    return m_image_url;
}

const char* MeFile::imageUrl() {
    return imageUrl(0, 0);
}

const char* MeFile::imageCropUrl(int x, int y, int width, int height) {
    memset(m_image_url, 0, URL_SIZE);
    Crypt crypt;
    snprintf(m_image_url, URL_SIZE, "x-oss-process=image/crop,x_%d,y_%d,w_%d,h_%d", x, y, width, height);
    char *cryptoParam = MeCloud::shareInstance()->crypto(m_image_url, (int)strlen(m_image_url));
    memset(m_image_url, 0, URL_SIZE);
    snprintf(m_image_url, URL_SIZE, "%sfile/download/%s?%s", MeCloud::shareInstance()->restUrl(), objectId(), cryptoParam);
    free(cryptoParam);
    return m_image_url;
}

