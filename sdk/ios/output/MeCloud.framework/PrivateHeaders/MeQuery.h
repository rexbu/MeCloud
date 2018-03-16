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

#define ME_QUERY_DEFAULT_VALUE      0

typedef enum {
    COUNT = 1,
    ID,
    LIST,
    JSONOBJECT
} MEAGGREGATEMETHOD;

class MeAggregateObject: public JSONObject {
public:
    MeAggregateObject(const char* className);
    MeAggregateObject(MeAggregateObject *aggregateObject);
    MeAggregateObject(const MeAggregateObject &aggregateObject);
    ~MeAggregateObject();
    inline const char* classname() { return m_classname; }

    void whereEqualTo(const char* key, const char* val);
    void whereEqualTo(const char* key, int val);
    void whereNotEqualTo(const char* key, const char* val);
    void whereNotEqualTo(const char* key, int val);
    void whereGreater(const char *key, const char *val);
    void whereGreater(const char *key, int val);
    void whereLess(const char *key, const char *val);
    void whereLess(const char *key, int val);
    void setResponseKey(const char* keyValue);
    void setDistinctKey(const char* keyValue);
    void setMethod(MEAGGREGATEMETHOD method);
    inline MEAGGREGATEMETHOD aggregateMethod() { return m_method; };
    inline const char* responseKey() { return m_keyValue; };
    inline const char* distinctKey() { return m_distinct_keyValue; };

    MeAggregateObject *next = NULL;
protected:
    char m_classname[ME_CLASS_SIZE];
    const char *m_keyValue = NULL;
    const char *m_distinct_keyValue = NULL;
    MEAGGREGATEMETHOD m_method;
};

class MeQuery: public JSONObject {
public:
    // 单表查询需要调用该构造函数
    MeQuery(const char* className);
    // 查找聚合类数据用该构造函数创建MeAuery,所谓的聚合类有count，sum，average等相关数据，难以理解参照关系型数据库
    MeQuery();
    ~MeQuery();
    
    inline const char *url() { return m_url; }
    
    // 单表查询
    void whereEqualTo(const char* key, const char* val);
    void whereEqualTo(const char* key, int value);
    void whereNotEqualTo(const char* key, const char* val);
    void whereNotEqualTo(const char* key, int value);
    void whereGreater(const char *key, const char *val);
    void whereGreater(const char *key, int val);
    void whereLess(const char *key, const char *val);
    void whereLess(const char *key, int val);
    void whereEqualOr(const char* key, const char* value);
    void whereEqualOr(const char* key, int value);
    void selectKeys(const char* keys[], int num);
    void addSelectKey(const char* key);
    void addNotSelectKey(const char* key);
    void addAscendSortKeys(const char* key);
    void addDescendSortKeys(const char* key);
    void addLimit(long count);
    void addStartId(const char* startId);
    
    // 聚合类查询,如果查询中包含此查询就不会执行单表查询，此条件和单表查询条件互斥
    // 增加一个聚合查询
    void addAggregateObject(MeAggregateObject *aggregate);
    // 会将已有的聚合查询全部清空，然后再新增一个聚合查询
    void setAggregateObject(MeAggregateObject *aggregate);
    
	void get(const char* objectId, MeCallback* callback);
	void find(MeCallback* callback);
    
#ifdef __IOS__
    void find(MeCallbackBlock block);
    void get(const char* objectId, MeCallbackBlock block);
#endif
    
protected:
    char m_classname[ME_CLASS_SIZE] = "\0";
    char m_url[URL_SIZE] = "\0";
    char m_param[URL_SIZE] = "\0";
    
    long limit = ME_QUERY_DEFAULT_VALUE;
    const char* startId = NULL;
    JSONObject      m_select_keys;
    JSONObject      m_filter_keys;
    JSONArray       *jsonArray = NULL;
    vector<MeAggregateObject *> aggregateVector;
#ifdef __IOS__
    MeCallbackBlock     m_callback;
#else
#endif
private:
    void composeParam(const char *key, const char *value, const char* connector);
    void formatParam();
    void requestAggregate(MeCallback* callback);
    void requsetSingle(MeCallback* callback);
    void clear();
};

#endif
