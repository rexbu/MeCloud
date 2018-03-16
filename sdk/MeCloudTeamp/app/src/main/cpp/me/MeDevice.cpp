/**
 * file :	MeDevice.cpp
 * author :	Rex
 * create :	2017-04-04 16:57
 * func : 
 * history:
 */

#include "MeDevice.h"
#include "McDevice.h"
#include "McBasic.h"

MeDevice* MeDevice::m_current_device = NULL;

MeDevice::MeDevice():
MeObject("Device"){
    // 设备
#ifdef __IOS__
    put("bundleId", mc::bundle_id());
#else
    put("package", mc::package_name());
#endif
    put("device", mc::device_version());
    put("system", mc::system_version());
    put("deviceId", mc::device_id());
    // deviceId为唯一索引
    addUniqueKey("deviceId");
    
    // 回调设置
    m_object = this;
    MeCallback::m_classname = MeObject::m_classname;
}

MeDevice::MeDevice(JSONObject* obj):MeObject("Device", obj){
    // 回调设置
    m_object = this;
    MeCallback::m_classname = MeObject::m_classname;
}

#pragma --mark "注册设备与退出"
void MeDevice::login(MeCallback* callback){
    // 处理唯一索引
    char unique[512] = {0};
    for (int i=0; i<m_unique_key.size(); i++) {
        snprintf(unique, sizeof(unique)-strlen(unique)-1, "unique=%s&", m_unique_key[i].c_str());
    }
    if (strlen(unique) > 0) {
        // 去掉最后一个&
        unique[strlen(unique)-1] = '\0';
        snprintf(m_url, sizeof(m_url), "%s%s?%s", MeCloud::shareInstance()->classUrl(), MeObject::m_classname, unique);
    }
    else{
        snprintf(m_url, sizeof(m_url), "%s%s", MeCloud::shareInstance()->classUrl(), MeObject::m_classname);
    }
    
    m_callback = callback;
    MeCloud::shareInstance()->post(m_url, toString(), this);
}

void MeDevice::logout(){
    if (m_current_device!=NULL) {
        char path[1024];
        FileManager::remove(SharedPreferences::path("__CurrentDevice", path, sizeof(path)) );
        delete m_current_device;
        m_current_device = NULL;
    }
}

#pragma --mark "回调"
void MeDevice::done(MeObject* obj, MeException* err, uint32_t size){
    // 写currentUser
    if (err!=NULL) {
        m_callback->done(obj, err, size);
        return;
    }
    
    // 存储CurrentUser
    m_current_device = this;
    char path[1024];
    FileManager::remove(SharedPreferences::path("__CurrentDevice", path, sizeof(path)) );
    Crypt crypt;
    const char* str = toString();
    int64_t crysize = crypt.encrypt((byte*)str, (uint32_t)strlen(str));
    FileManager::write(path, crypt.bytes(), crysize);
    
    m_callback->done(obj, err, size);
}

MeDevice* MeDevice::currentDevice(){
    if (m_current_device == NULL) {
        SharedPreferences preference("__CurrentDevice");
        if (preference.exist()) {
            m_current_device = new MeDevice(&preference);
            return m_current_device;
        }
        
        return NULL;
    }
    
    return m_current_device;
}
