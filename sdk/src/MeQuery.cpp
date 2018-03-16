/**
 * file :	MeQuery.cpp
 * author :	bushaofeng
 * create :	2016-08-27 01:09
 * func : 
 * history:
 */

#include "MeQuery.h"

MeAggregateObject::MeAggregateObject(const char* className) {
    bs_strcpy(m_classname, sizeof(m_classname), className);
}

MeAggregateObject::MeAggregateObject(MeAggregateObject *aggregateObject) {
    bs_strcpy(m_classname, sizeof(m_classname), aggregateObject->m_classname);
    if (m_root!=NULL) {
        cJSON_Delete(m_root);
    }

    m_root = cJSON_Duplicate(aggregateObject->getCJSON(), 1);
    m_keyValue = aggregateObject->m_keyValue;
    m_distinct_keyValue = aggregateObject->m_distinct_keyValue;
    m_method = aggregateObject->m_method;
    m_auto_destruct  = true;
}

MeAggregateObject::MeAggregateObject(const MeAggregateObject &aggregateObject) {
    bs_strcpy(m_classname, sizeof(m_classname), aggregateObject.m_classname);
    if (m_root!=NULL) {
        cJSON_Delete(m_root);
    }
    
    m_root = cJSON_Duplicate(aggregateObject.getCJSON(), 1);
    m_keyValue = aggregateObject.m_keyValue;
    m_distinct_keyValue = aggregateObject.m_distinct_keyValue;
    m_method = aggregateObject.m_method;
    m_auto_destruct  = true;
}

MeAggregateObject::~MeAggregateObject() {
    
}

void MeAggregateObject::whereEqualTo(const char* key, const char* val) {
    put(key, val);
}

void MeAggregateObject::whereEqualTo(const char* key, int val) {
    put(key, val);
}

void MeAggregateObject::whereNotEqualTo(const char* key, const char* val) {
    JSONObject obj;
    obj.put("$ne", val);
    put(key, &obj);
}

void MeAggregateObject::whereNotEqualTo(const char* key, int val) {
    put(key, val);
}

void MeAggregateObject::whereGreater(const char *key, const char *val) {
    JSONObject obj;
    obj.put("$gt", val);
    put(key, &obj);
}

void MeAggregateObject::whereGreater(const char *key, int val) {
    JSONObject obj;
    obj.put("$gt", val);
    put(key, &obj);
}

void MeAggregateObject::whereLess(const char *key, const char *val) {
    JSONObject obj;
    obj.put("$lt", val);
    put(key, &obj);
}

void MeAggregateObject::whereLess(const char *key, int val) {
    JSONObject obj;
    obj.put("$lt", val);
    put(key, &obj);
}

void MeAggregateObject::setResponseKey(const char* keyValue) {
    m_keyValue = keyValue;
}

void MeAggregateObject::setDistinctKey(const char* keyValue) {
    m_distinct_keyValue = keyValue;
}

void MeAggregateObject::setMethod(MEAGGREGATEMETHOD method) {
    m_method = method;
}

#pragma --mark MeQuery
MeQuery::MeQuery(const char* className){
    bs_strcpy(m_classname, sizeof(m_classname), className);
}

MeQuery::MeQuery() {
}

void MeQuery::whereEqualTo(const char* key, const char* val) {
    put(key, val);
}

void MeQuery::whereEqualTo(const char* key, int val) {
    put(key, val);
}

void MeQuery::whereNotEqualTo(const char* key, const char* val) {
    JSONObject obj;
    obj.put("$ne", val);
    put(key, &obj);
}

void MeQuery::whereNotEqualTo(const char* key, int val) {
    JSONObject obj;
    obj.put("$ne", val);
    put(key, &obj);
}

void MeQuery::whereEqualOr(const char* key, const char* value) {
    if (!jsonArray) {
        jsonArray = new JSONArray();
    }
    JSONObject obj;
    obj.setDestruct(false);
    obj.put(key, value);
    jsonArray->append(&obj);
}

void MeQuery::whereEqualOr(const char* key, int value) {
    if (!jsonArray) {
        jsonArray = new JSONArray();
    }
    JSONObject obj;
    obj.setDestruct(false);
    obj.put(key, value);
    jsonArray->append(&obj);
}

void MeQuery::whereGreater(const char *key, const char *val) {
    JSONObject obj;
    obj.put("$gt", val);
    put(key, &obj);
}

void MeQuery::whereGreater(const char *key, int val) {
    JSONObject obj;
    obj.put("$gt", val);
    put(key, &obj);
}

void MeQuery::whereLess(const char *key, const char *val) {
    JSONObject obj;
    obj.put("$lt", val);
    put(key, &obj);
}

void MeQuery::whereLess(const char *key, int val) {
    JSONObject obj;
    obj.put("$lt", val);
    put(key, &obj);
}

void MeQuery::selectKeys(const char* keys[], int num) {
    for (int i=0; i<num; i++) {
        addSelectKey(keys[i]);
    }
}

void MeQuery::addSelectKey(const char* key){
    m_select_keys.put(key, 1);
}

void MeQuery::addNotSelectKey(const char* key){
    m_select_keys.put(key, 0);
}

void MeQuery::addAscendSortKeys(const char* key) {
    m_filter_keys.put(key, 1);
}

void MeQuery::addDescendSortKeys(const char* key) {
    m_filter_keys.put(key, -1);
}

void MeQuery::addLimit(long count) {
    limit = count;
}

void MeQuery::addStartId(const char* startId) {
    this->startId = startId;
}

void MeQuery::addAggregateObject(MeAggregateObject *aggregate) {
    MeAggregateObject *newAggregate = new MeAggregateObject(aggregate);
    aggregateVector.push_back(newAggregate);
}

void MeQuery::setAggregateObject(MeAggregateObject *aggregate) {
    clear();
    addAggregateObject(aggregate);
}

void MeQuery::get(const char* objectId, MeCallback* callback) {
    put("_id", objectId);
    snprintf(m_url, sizeof(m_url), "%s%s/%s", MeCloud::shareInstance()->classUrl(), m_classname, objectId);
    MeCloud::shareInstance()->get(m_url, callback);
}

void MeQuery::find(MeCallback* callback) {
    if (aggregateVector.size() > 0) {
        requestAggregate(callback);
    } else {
        requsetSingle(callback);
    }
}

void MeQuery::requsetSingle(MeCallback* callback) {
    if (jsonArray && jsonArray->size() > 0) {
        put("$or", jsonArray);
    }
    
    char* whereParam = (char *)toString();
    if (empty()) {
        whereParam = "{}";
    }
    
    composeParam("where", (const char *)whereParam, "");
    if (!m_select_keys.empty()) {
        composeParam("keys", m_select_keys.toString(), "&");
    }
    
    if (!m_filter_keys.empty()) {
        composeParam("sort", m_filter_keys.toString(), "&");
    }
    
    if (limit > ME_QUERY_DEFAULT_VALUE) {
        char limitParam[128] = "\0";
        snprintf(limitParam, sizeof(limitParam), "%ld", limit);
        composeParam("limit", limitParam, "&");
    }
    
    if (startId) {
        char startParam[128] = "\0";
        snprintf(startParam, sizeof(startParam), "%s", startId);
        composeParam("startId", startParam, "&");
    }
    
    formatParam();
    char *param = MeCloud::shareInstance()->crypto(m_param, (int)strlen(m_param));
    snprintf(m_url, sizeof(m_url)-1, "%s%s?%s", MeCloud::shareInstance()->classUrl(), m_classname, param);
    free(param);
    
    MeCloud::shareInstance()->get(m_url, callback);
}

void MeQuery::requestAggregate(MeCallback* callback) {
    JSONArray *aggregateArray = new JSONArray();
    for (vector<MeAggregateObject *>::iterator iter = aggregateVector.begin(); iter != aggregateVector.end(); iter++) {
        JSONObject *aggregate = new JSONObject();
        aggregate->setDestruct(false);
        aggregate->put("condition", *iter);
        aggregate->put("key", (*iter)->responseKey());
        aggregate->put("method", (*iter)->aggregateMethod());
        aggregate->put("classname", (*iter)->classname());
        const char *distinctKey = (*iter)->distinctKey();
        if (distinctKey) {
          aggregate->put("distinct", distinctKey);
        }
        aggregateArray->append(aggregate);
        delete aggregate;
    }
    
    composeParam("where", aggregateArray->toString(), "");
    
    formatParam();
    char *param = MeCloud::shareInstance()->crypto(m_param, (int)strlen(m_param));
    snprintf(m_url, sizeof(m_url)-1, "%squery/?%s", MeCloud::shareInstance()->restUrl(), param);
    free(param);
    delete aggregateArray;
    
    MeCloud::shareInstance()->get(m_url, callback);
}

void MeQuery::composeParam(const char *key, const char *value, const char* connector) {
    snprintf(m_param, sizeof(m_param)-1, "%s%s%s=%s", m_param, connector, key, value);
}

void MeQuery::formatParam() {
    int k = 0;
    for (int i = 0; i < strlen(m_param); ++i) {
        if (m_param[i] != '\n' &&  m_param[i] != '\t' && m_param[i] != ' ')  {
            m_param[k++] = m_param[i];
        }
    }
    m_param[k] = '\0';
}

void MeQuery::clear() {
    for (vector<MeAggregateObject *>::iterator iter = aggregateVector.begin(); iter != aggregateVector.end(); iter++) {
        if (*iter != NULL) {
            delete *iter;
            *iter = NULL;
        }
    }

    aggregateVector.clear();
}

MeQuery::~MeQuery() {
    clear();
    delete jsonArray;
    jsonArray = NULL;
}
