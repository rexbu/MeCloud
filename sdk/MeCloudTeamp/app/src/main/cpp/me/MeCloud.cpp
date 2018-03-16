/**
 * file :	MeCloud.cpp
 * author :	bushaofeng
 * create :	2016-08-27 01:09
 * func : 
 * history:
 */

#include "bs.h"
#include "MeCloud.h"

MeCloud* MeCloud::m_instance = NULL;
const char* MeCloud::m_version = "1.0";

#if DEBUG
char MeCloud::m_base_url[URL_SIZE] = "http://n01.me-yun.com:8001/";
#else
char MeCloud::m_base_url[URL_SIZE] = "http://n01.me-yun.com:8001/";
#endif

void MeCloud::setBaseUrl(const char* url){
    snprintf(m_base_url, sizeof(m_base_url), "%s", url);
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
    }
    
	return m_instance;
}

MeCloud::MeCloud(const char* appId, const char* appKey){
    if (appId != NULL && appKey != NULL) {
        m_http_session.addHttpHeader("X-MeCloud-AppId", appId);
        m_http_session.addHttpHeader("X-MeCloud-AppKey", appKey);
    }
	
	m_http_session.addHttpHeader("X-MeCloud-Version", m_version);
}

void MeCloud::get(const char* url, MeCallback* callback){
	m_http_session.get(url, callback);
}
void MeCloud::post(const char* url, const char* body, MeCallback* callback){
	m_http_session.post(url, body, (uint32_t)strlen(body), callback);
}
void MeCloud::put(const char* url, const char* body, MeCallback* callback){
	m_http_session.put(url, body, (uint32_t)strlen(body), callback);
}
void MeCloud::del(const char* url, MeCallback* callback){
	m_http_session.del(url, callback);
}
void MeCloud::download(const char* url, const char* path, DownCallback* callback){
	m_http_session.download(url, path, callback);
}

const char* MeCloud::restUrl(){
	snprintf(m_url, URL_SIZE, "%s%s/", m_base_url, m_version);
	return m_url;
}
const char* MeCloud::classUrl(){
	snprintf(m_url, URL_SIZE, "%s%s/classes/", m_base_url, m_version);
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
