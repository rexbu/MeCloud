/**
 * file :	MeCloud.h
 * author :	bushaofeng
 * create :	2016-08-27 01:09
 * func : 
 * history:
 */

#ifndef	__MECLOUD_H_
#define	__MECLOUD_H_

#include "McHttp.h"
#include "MeCallback.h"
#include "JSON.h"
#include <map>
#include <vector>
#include <string>

using namespace mc;
class MeException: public JSONObject{
public:
    MeException(){}
    MeException(const char* str):JSONObject(str){}
    
    int errCode(){
        return intValue("errCode");
    }
    const char* errMsg(){
        return stringValue("errMsg");
    }
    const char* info(){
        return stringValue("info");
    }
};

class MeCloud{
public:
    static void initialize(const char* appId = NULL, const char* appKey = NULL);
    static MeCloud* shareInstance();
    
    static void setBaseUrl(const char* url);
    static void setTimeout(uint32_t timeout);
    static void showLog(bool show);
    inline  HttpSession* httpSession(){ return &m_http_session; }
    inline static bool isShowedLog(){ return m_log; }
    void clearCookie();
    const char* cookie();
    
    MeCloud(const char* appId, const char* appKey);
    void addHttpHeader(const char* key, const char* value);

    char* crypto(const char *input, int in_size);
    char* decrypt(const char *input, int in_size);
    unsigned char* crypto(const unsigned char *input, int in_size);
    unsigned char* decrypt(const unsigned char *input, int in_size);
    
    void get(const char* url, map<string, string> param, MeCallback* callback = NULL);
    void get(const char* url, MeCallback* callback = NULL);
    void post(const char* url, const char* body, MeCallback* callback = NULL);
    void put(const char* url, const char* body, MeCallback* callback = NULL);
    void del(const char* url, MeCallback* callback = NULL);
    void download(const char* url, const char* path, HttpFileCallback* callback = NULL);
    void upload(const char* url, const char* path, vector<pair<string, string> > auth, HttpFileCallback* callback = NULL);
    void upload(const char* url, uint8_t *data, int size, vector<pair<string, string> > auth, HttpFileCallback* callback = NULL);
    void postData(const char* url, uint8_t *data, int size, HttpFileCallback* callback);
	// 以下4个函数不能同时调用，第二个会把第一个抹掉
    const char* baseUrl();
    const char* restUrl();
    const char* classUrl();
    const char* userUrl();
    const char* fileUrl();
  
    static const char* m_version;
    static char m_base_url[URL_SIZE];
    
    void saveJSONToCache(JSONObject *json, const char *key);
    JSONObject* getJSONFromCache(const char *key);
    void removeJSONFromCache(const char *key);
    
protected:
    static MeCloud*  m_instance;
    static bool     m_log;
    HttpSession  m_http_session;
    char      m_url[URL_SIZE];
    char*      m_app_id;
    char*      m_app_key;
};
#endif
