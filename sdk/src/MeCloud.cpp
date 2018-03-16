/**
 * file :	MeCloud.cpp
 * author :	bushaofeng
 * create :	2016-08-27 01:09
 * func : 
 * history:
 */

#include "bs.h"
#include "MeCloud.h"
#include <iterator>

MeCloud* MeCloud::m_instance = NULL;
const char* MeCloud::m_version = "1.0";
bool MeCloud::m_log = false;

#if DEBUG
char MeCloud::m_base_url[URL_SIZE] = "http://n01.me-yun.com:8000/";
#else
char MeCloud::m_base_url[URL_SIZE] = "http://n01.me-yun.com:8000/";
#endif

void MeCloud::setBaseUrl(const char* url) {
    snprintf(m_base_url, sizeof(m_base_url), "%s", url);
}

void MeCloud::setTimeout(uint32_t timeout) {
    HttpSession::setTimeout(timeout);
}

void MeCloud::showLog(bool show) {
    m_log = show;
    HttpSession::showLog(show);
}

void MeCloud::initialize(const char* appId, const char* appKey){
    if (m_instance==NULL)
    {
      m_instance = new MeCloud(appId, appKey);
    }
}

MeCloud* MeCloud::shareInstance(){
    if (m_instance==NULL) {
        m_instance = new MeCloud(NULL, NULL);
        
        bs_log_init("stdout");
    }
    return m_instance;
}

MeCloud::MeCloud(const char* appId, const char* appKey){
    if (appId != NULL && appKey != NULL) {
        m_http_session.addHttpHeader("X-MeCloud-AppId", appId);
        m_http_session.addHttpHeader("X-MeCloud-AppKey", appKey);
    }
    m_http_session.addHttpHeader("X-MeCloud-Version", m_version);
    
#if DEBUG
    m_http_session.addHttpHeader("X-MeCloud-Debug", "1");
#else
#endif
}

void MeCloud::addHttpHeader(const char* key, const char* value) {
    m_http_session.addHttpHeader(key, value);
}

const char* MeCloud::cookie() {
    return m_http_session.cookie();
}

// 清理cookie
void MeCloud::clearCookie() {
    m_http_session.clearCookie();
}

// 内存需要外面释放
char* MeCloud::crypto(const char *input, int in_size) {
    Crypt crypt;
    char *output;
#if DEBUG
    output = (char *)malloc(in_size + 1);
    memset(output, 0, in_size + 1);
    memcpy(output, input, in_size + 1);
#else
    int outSize = in_size * 2 + 1;
    output = (char *)malloc(outSize);
    crypt.crypto((char *)input, in_size, output, outSize);
#endif
    return output;
}

// 内存需要外面手动释放
char* MeCloud::decrypt(const char *input, int in_size) {
    Crypt crypt;
    char *decryOutput;
#if DEBUG
    decryOutput = (char *)malloc(in_size + 1);
    memset(decryOutput, 0, in_size + 1);
    memcpy(decryOutput, input, in_size + 1);
#else
    int outSize = in_size / 2 + 1;
    decryOutput = (char *)malloc(outSize);
    crypt.decrypt((char *)input, in_size, decryOutput, outSize);
#endif
    
    return decryOutput;
}

unsigned char* MeCloud::crypto(const unsigned char *input, int in_size) {
    Crypt crypt;
    int outSize = in_size + 1;
    unsigned char *output = (unsigned char *)malloc(outSize);
    crypt.crypto((unsigned char *)input, in_size, output, outSize);
    return output;
}

unsigned char* MeCloud::decrypt(const unsigned char *input, int in_size) {
    Crypt crypt;
    int outSize = in_size + 1;
    unsigned char *decryOutput = (unsigned char *)malloc(outSize);
    crypt.decrypt((unsigned char *)input, in_size, decryOutput, outSize);
    return decryOutput;
}

void MeCloud::get(const char* url, MeCallback* callback){
	m_http_session.get(url, callback);
}

void MeCloud::get(const char* url, map<string, string> param, MeCallback* callback) {
    char newUrl[URL_SIZE];
    snprintf(newUrl, sizeof(newUrl), "%s", url);
    for (map<string, string>::iterator iter = param.begin(); iter != param.end(); iter++) {
        char *keyOutput = crypto(iter->first.c_str(), (int)strlen(iter->first.c_str()));
        char *valueOutput = crypto(iter->second.c_str(), (int)strlen(iter->second.c_str()));
        if (iter == param.begin()) {
            snprintf(newUrl, sizeof(newUrl), "%s?%s=%s", newUrl, keyOutput, valueOutput);
        } else {
            snprintf(newUrl, sizeof(newUrl), "%s&%s=%s", newUrl, keyOutput, valueOutput);
        }
        free(keyOutput);
        free(valueOutput);
    }
    
    m_http_session.get(newUrl, callback);
}

void MeCloud::post(const char* url, const char* body, MeCallback* callback){
    const char *content = body;
    if (!content) {
        content = "";
    }
    
    char *output = crypto(content, (int)strlen(content));
    m_http_session.post(url, output, (uint32_t)strlen(output), callback);
    free(output);
}
void MeCloud::put(const char* url, const char* body, MeCallback* callback){
    char *output = crypto(body, (int)strlen(body));
    m_http_session.put(url, output, (uint32_t)strlen(output), callback);
    free(output);
}
void MeCloud::del(const char* url, MeCallback* callback){
	m_http_session.del(url, callback);
}
void MeCloud::download(const char* url, const char* path, HttpFileCallback* callback){
	m_http_session.download(url, path, callback);
}
void MeCloud::upload(const char* url, const char* path, vector<pair<string, string> > auth, HttpFileCallback* callback){
    m_http_session.upload(url, path, auth, callback);
}

void MeCloud::upload(const char* url, uint8_t *data, int size, vector<pair<string, string> > auth, HttpFileCallback* callback) {
    m_http_session.upload(url, data, size, auth, callback);
}

void MeCloud::postData(const char* url, uint8_t *data, int size,HttpFileCallback* callback) {
    device_id();
    m_http_session.postData(url, data, size, callback);
}

const char* MeCloud::baseUrl(){
    return m_base_url;
}

const char* MeCloud::restUrl(){
	snprintf(m_url, URL_SIZE, "%s%s/", m_base_url, m_version);
	return m_url;
}
const char* MeCloud::classUrl(){
	snprintf(m_url, URL_SIZE, "%s%s/class/", m_base_url, m_version);
	return m_url;
}
const char* MeCloud::userUrl(){
	snprintf(m_url, URL_SIZE, "%s%s/user/", m_base_url, m_version);
	return m_url;
}
const char* MeCloud::fileUrl(){
	snprintf(m_url, URL_SIZE, "%s%s/file/", m_base_url, m_version);
	return m_url;
}

void MeCloud::saveJSONToCache(JSONObject *json, const char *key) {
    char path[1024];
    char file_name[BS_MD5_STRLEN + 1];
    bs_sign(key, file_name);
    FileManager::remove(SharedPreferences::path(file_name, path, sizeof(path)));
    Crypt crypt;
    const char *str = json->toString();
    int64_t crysize = crypt.encryptByCompress((byte*)str, (uint32_t)strlen(str));
    FileManager::write(path, crypt.bytes(), crysize);
}

JSONObject* MeCloud::getJSONFromCache(const char *key) {
    char file_name[BS_MD5_STRLEN + 1];
    bs_sign(key, file_name);
    SharedPreferences fileContentPreference(file_name);
    if (fileContentPreference.exist()) {
        JSONObject *json = new JSONObject(&fileContentPreference);
        return json;
    }
    
    return NULL;
}

void MeCloud::removeJSONFromCache(const char *key) {
    char path[1024];
    char file_name[BS_MD5_STRLEN + 1];
    bs_sign(key, file_name);
    FileManager::remove(SharedPreferences::path(file_name, path, sizeof(path)));
}
