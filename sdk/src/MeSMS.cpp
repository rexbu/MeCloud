/**
 * file :	MeSMS.cpp
 * author :	Rex
 * create :	2017-04-04 11:48
 * func : 
 * history:
 */

#include "MeSMS.h"

const char* MeSMS::getSMSUrl(const char* phone) {
    char m_url[URL_SIZE];
    snprintf(m_url, sizeof(m_url), "%ssms/%s", MeCloud::shareInstance()->baseUrl(), phone);
    return m_url;
}

void MeSMS::sendSMS(const char* phone, MeCallback *callback) {
    char m_url[URL_SIZE];
    snprintf(m_url, sizeof(m_url), "%ssms/%s", MeCloud::shareInstance()->baseUrl(), phone);
    MeCloud::shareInstance()->get(m_url, callback);
}
