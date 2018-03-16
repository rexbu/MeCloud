/**
 * file :	MeQuery.h
 * author :	bushaofeng
 * create :	2016-08-27 01:09
 * func : 
 * history:
 */

#ifndef	__MEQUERY_H_
#define	__MEQUERY_H_

#include "MeObject.h"

class MeQuery: public JSONObject{
public:
	MeQuery(const char* className);
	void whereEqualTo(const char* key, const char* val);
	void whereNotEqualTo(const char* key, const char* val);
    void whereEqualTo(const char* key, int value);
    void whereNotEqualTo(const char* key, int value);
	void selectKeys(const char* keys[], int num);
    void addSelectKey(const char* key);
    void addNotSelectKey(const char* key);

	void get(const char* objectId, MeCallback* callback);
	void find(MeCallback* callback);
    
#ifdef __IOS__
    void find(MeCallbackBlock block);
    void get(const char* objectId, MeCallbackBlock* callback);
#endif
    
protected:
    char            m_classname[ME_CLASS_SIZE];
    char            m_url[URL_SIZE];
    
    JSONObject      m_select_keys;
#ifdef __IOS__
    MeCallbackBlock     m_callback;
#else
#endif
};
#endif
