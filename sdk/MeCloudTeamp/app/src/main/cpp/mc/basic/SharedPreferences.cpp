/**
 * file :	SharedPreferences.cpp
 * author :	Rex
 * create :	2016-10-21 15:45
 * func : 
 * history:
 */

#include "SharedPreferences.h"
#include "McZip.h"

using namespace mc;

static char g_root_path[1024];
const char* SharedPreferences::m_root_path = NULL;

SharedPreferences::SharedPreferences(const char* name){
    if (name != NULL) {
        path(name, m_path, sizeof(m_path));
        if(open(m_path)){
            Crypt crypt;
            byte* b = File::read(m_path);
            crypt.decrypt(b, (uint32_t)File::size());
            parse((const char*)crypt.bytes());
        }
    }
}

bool SharedPreferences::commit(){
    const char* b = toString();
    Crypt crypt;
    int64_t size = crypt.encrypt((byte*)b, (uint32_t)strlen(b));
    FileManager::write(m_path, crypt.bytes(), (uint32_t)size);
    return true;
}

const char* SharedPreferences::path(const char* name, char* path, uint32_t size){
    if (m_root_path == NULL) {
        snprintf(g_root_path, sizeof(g_root_path), "%s/__shared_prefs", FileManager::rootPath());
        // 如果不存在则创建文件夹
        if (!FileManager::exist(g_root_path)) {
            FileManager::mkdir(g_root_path);
        }
        m_root_path = g_root_path;
    }
    
    memset(path, 0, size);
    snprintf(path, size-1, "%s/%s", m_root_path, name);
    return path;
}
