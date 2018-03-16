/**
 * file :	File.cpp
 * author :	bushaofeng
 * create :	2016-08-27 01:01
 * func : 
 * history:
 */

#include <iostream>
#include <string>
#include "McFile.h"
#include "McDevice.h"

using namespace mc;

#pragma --mark "文件夹"

Folder::Folder(const char* path){
    m_dir = opendir(path);
    m_path = path;
}

const char* Folder::type(const char* type){
    if (m_dir==NULL) {
        return NULL;
    }
    
    struct dirent*  ent;
    size_t len = strlen(type);
    while((ent=readdir(m_dir))!=NULL){
        // ios下为namlen, android下为rectlen
        size_t namelen = strlen(ent->d_name);
        if (memcmp(type, &ent->d_name[namelen - len], len)==0) {
            snprintf(m_buffer, sizeof(m_buffer), "%s/%s", m_path.c_str(), ent->d_name);
            return m_buffer;
        }
    }
    return NULL;
}
const char* Folder::name(const char* name){
    struct dirent*  ent;
    size_t len = strlen(name);
    while((ent=readdir(m_dir))!=NULL){
        if (memcmp(name, ent->d_name, len)==0) {
            snprintf(m_buffer, sizeof(m_buffer), "%s/%s", m_path.c_str(), ent->d_name);
            return m_buffer;
        }
    }
    return NULL;
}

#pragma --mark "文件"

File::File(){
    memset(&m_info, 0, sizeof(m_info));
    string_init(&m_path);
    m_exist = false;
    m_bytes = NULL;
    m_bytes_size = 0;
}

File::File(const char* path){
    memset(&m_info, 0, sizeof(m_info));
    string_init(&m_path);
    m_exist = false;
    m_bytes = NULL;
    m_bytes_size = 0;
    open(path);
}

bool File::open(const char* path){
    string_set(&m_path, path);
    int st = stat(path, &m_info);
    if (st != 0) {
        return false;
    }
    
    m_exist = !S_ISDIR(m_info.st_mode);
    return m_exist;
}

byte* File::read(const char* path){
    if (path==NULL && !m_exist) {
        return NULL;
    }
    if (path!=NULL && !open(path)) {
        return NULL;
    }
    
    if (m_bytes_size < size()+1) {
        if (m_bytes!=NULL) {
            free(m_bytes);
        }
        // 如果string, 最后一个字节加\0
        m_bytes = (byte*)malloc(size()+1);
        m_bytes_size = size()+1;
        m_bytes[size()] = '\0';
    }
    
    int fd = ::open(m_path.mem, O_RDONLY);
    long s = ::read(fd, m_bytes, size());
    close(fd);
    
    if (s!=size()) {
        return NULL;
    }
    
    return m_bytes;
}

/// 和FileManager区别是这里只判断文件，不判断文件夹
bool File::exist(){
    return m_exist;
}

/// 文件大小
size_t File::size(){
    return m_info.st_size;
}

#pragma --mark "文件管理器"
FileManager* FileManager::m_instance = NULL;

#ifdef __IOS__
const char* FileManager::m_root_path = NULL;
const char* FileManager::m_resource_path = NULL;
#elif __ANDROID__
const char* FileManager::m_root_path = NULL;
const char* FileManager::m_resource_path = NULL;

static char g_root_path[1024];
static char g_res_path[1024];
const char* FileManager::resourcePath(){
    if (m_resource_path==NULL)
    {
        snprintf(g_res_path, sizeof(g_res_path), "%s/__resource", rootPath());
        if (!exist(g_res_path)) {
            err_log("mkdir resource: %s", g_res_path);
            mkdir(g_res_path);
        }
        m_resource_path = g_res_path;
    }
    
    return m_resource_path;
}

const char* FileManager::rootPath(){
    if (m_root_path==NULL)
    {
        snprintf(g_root_path, sizeof(g_root_path), "/data/data/%s", g_package_name);
        m_root_path = g_root_path;
    }

    return m_root_path;
}

#endif
char FileManager::m_path_buffer[1024];
char FileManager::m_tmp_path[1024];

FileManager* FileManager::shareInstance(){
    if (m_instance == NULL) {
        m_instance = new FileManager();
    }
    return m_instance;
}

bool FileManager::isFile(const char* path){
    struct stat info;
    int st = stat(path, &info);
    if (st!=0) {
        return false;
    }
    return !S_ISDIR(info.st_mode);
}

bool FileManager::isDir(const char* path){
    struct stat info;
    int st = stat(path, &info);
    if (st!=0) {
        return false;
    }
    return S_ISDIR(info.st_mode);
}

bool FileManager::exist(const char* path){
    /*
     06     检查读写权限
     04     检查读权限
     02     检查写权限
     01     检查执行权限
     00     检查文件的存在性
     */
    return access(path, 0)==0;
}
void FileManager::mkdir(const char* dir){
    ::mkdir(dir, S_IRWXU);
}
void FileManager::move(const char* from, const char* to){
    rename(from, to);
}
void FileManager::remove(const char *path){
    int st;
    if (isDir(path))
    {
        DIR * dir = NULL;  
        struct dirent *ptr;  
        dir = opendir(path);
        while((ptr = readdir(dir)) != NULL){
            if (strcmp(ptr->d_name, ".")==0 || strcmp(ptr->d_name, "..")==0){
                continue;
            }
            else{
                char child_buf[1024];
                snprintf(child_buf, sizeof(child_buf), "%s/%s", path, ptr->d_name); 
                if (isDir(child_buf))
                {
                    remove(child_buf);
                }
                else{
                    st = ::remove(child_buf);
                }
            }
        }
        rmdir(path);
    }
    else{
        st = ::remove(path);
        if (st != 0) {
            err_log("remove error[%d]", st);
        }
    } 
}
int FileManager::write(const char* path, void* data, size_t size){
//    int fd = ::open(path, O_RDWR|O_CREAT);
//    if (fd <= 0) {
//        err_log("open file error[%d]", errno);
//        return -1;
//    }
//    
//    size_t s = ::write(fd, data, size);
//    int rs = fsync(fd);  // 刷新文件描述符
//    if (s<=0 || rs<0) {
//        err_log("write file error: size[%lu] sync[%d] errno[%d]", s, rs, errno);
//    }
//    close(fd);
//    
//    return (int)s;
    FILE* file = fopen(path, "wb+");
    if (file==NULL) {
        err_log("open file error[%d]", errno);
        return -1;
    }
    
    size_t s = fwrite(data, size, 1, file);
    fflush(file);
    fclose(file);
    return (int)s;
}

const char* FileManager::resource(const char* file, char* path, uint32_t size){
    char* buffer = m_path_buffer;
    uint32_t buffer_size = sizeof(m_path_buffer);
    
    if (path!=NULL && size>0) {
        buffer = path;
        buffer_size = size;
    }

    snprintf(buffer, buffer_size, "%s/%s", resourcePath(), file);
// #ifdef __ANDROID__
    
//     // 从asset目录拷贝到__resource目录
//     Resource res;
//     if(res.read(file)<0){
//         return NULL;
//     }
    
//     write(buffer, res.bytes(), res.size());
//     return buffer;

// #else 
    if (exist(buffer)) {
        return buffer;
    }
    else{
        return NULL;
    }
//#endif
}

const char* FileManager::tempPath(){
    snprintf(m_tmp_path, sizeof(m_tmp_path)-1, "%s/__tmp", rootPath());
    if (!exist(m_tmp_path)) {
        mkdir(m_tmp_path);
    }
    
    return m_tmp_path;
}
