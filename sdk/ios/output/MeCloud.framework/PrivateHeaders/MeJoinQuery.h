//
//  MeJoinQuery.h
//  MeCloud
//
//  Created by super on 2017/9/17.
//  Copyright © 2017年 Rex. All rights reserved.
//

#ifndef MeJoinQuery_h
#define MeJoinQuery_h

#include "MeQuery.h"
#include <vector>
#include <iterator>

class MeJoinQuery: public JSONObject {
public:
    MeJoinQuery(const char *classname, bool nestQuery = false);
    ~MeJoinQuery();
    
    inline const char *url() { return m_url; }
    
    void addSelectKey(const char *key);
    void addNotSelectKey(const char *key);
    void addLimit(int count);
    // 确保加入的先后顺序，级别越高，理解关系型中的数据库select排序顺序
    void addAscend(const char *key);
    void addDescend(const char *key);
    //暂时不要
    void addUnwind(const char *key);
    
    void matchEqualTo(const char *key, const char *val);
    void matchEqualTo(const char *key, int val);
    void matchGreater(const char *key, const char *val);
    void matchGreater(const char *key, int val);
    void matchLess(const char *key, const char *val);
    void matchLess(const char *key, int val);
    void addForeignTable(const char *fromTable, const char *foreignKey, const char *localKey, const char* document);
    
    void addMeJoinQuery(MeJoinQuery *joinQuery);
    
    void find(MeCallback* callback);
protected:
    char m_classname[ME_CLASS_SIZE];
    char m_url[URL_SIZE];
    
    // 嵌套查询
    bool m_nest_query = false;

    long m_limit = ME_QUERY_DEFAULT_VALUE;
    long m_aggregate_count = ME_QUERY_DEFAULT_VALUE;
    JSONObject *m_match = NULL;
    JSONObject *m_select_keys = NULL;
    JSONObject *m_sort_keys = NULL;
    JSONArray *m_lookup = NULL;
    char *unwindKey = NULL;
    
    MeJoinQuery *next = NULL;
    vector<MeJoinQuery *> m_child_querys;
private:
    string_t* getjoinKey(const char* key);
    void composeQueryArray(MeJoinQuery *query, JSONArray *jsonArray);
};

#endif /* MeJoinQuery_h */
