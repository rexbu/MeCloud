//
//  MeJoinQuery.cpp
//  MeCloud
//  区别于MeQuery, MeQuery只能做单表查询，MeJoinQuery能做多表联合查询
//  Created by super on 2017/9/17.
//  Copyright © 2017年 Rex. All rights reserved.
//

#include "MeJoinQuery.h"

MeJoinQuery::MeJoinQuery(const char *classname, bool nestQuery) {
    memset(m_classname, 0, sizeof(m_classname));
    bs_strcpy(m_classname, sizeof(m_classname), classname);
    m_nest_query = nestQuery;
}

string_t *MeJoinQuery::getjoinKey(const char *key) {
    string_t *joinKey = bs_new(string);
    if (m_nest_query) {
        string_append(joinKey, m_classname);
        string_append(joinKey, ".");
        string_append(joinKey, key);
    } else {
        string_append(joinKey, key);
    }

    return joinKey;
}

void MeJoinQuery::addSelectKey(const char *key) {
    if (!m_select_keys) {
        m_select_keys = new JSONObject();
    }

    string_t *joinKey = getjoinKey(key);
    m_select_keys->put(joinKey->mem, 1);
    bs_delete(joinKey);
}

void MeJoinQuery::addNotSelectKey(const char *key) {
    if (!m_select_keys) {
        m_select_keys = new JSONObject();
    }

    string_t *joinKey = getjoinKey(key);
    m_select_keys->put(joinKey->mem, 0);
    bs_delete(joinKey);
}

void MeJoinQuery::addLimit(int count) {
    m_limit = count;
}

void MeJoinQuery::addAscend(const char *key) {
    if (!m_sort_keys) {
        m_sort_keys = new JSONObject();
    }

    string_t *joinKey = getjoinKey(key);
    m_sort_keys->put(joinKey->mem, 1);
    bs_delete(joinKey);
}

void MeJoinQuery::addDescend(const char *key) {
    if (!m_sort_keys) {
        m_sort_keys = new JSONObject();
    }

    string_t *joinKey = getjoinKey(key);
    m_sort_keys->put(joinKey->mem, -1);
    bs_delete(joinKey);
}

void MeJoinQuery::addUnwind(const char *key) {

}

void MeJoinQuery::matchEqualTo(const char *key, const char *val) {
    if (!m_match) {
        m_match = new JSONObject();
    }

    string_t *joinKey = NULL;
    if (!strcmp("_id", key)) {
        joinKey = getjoinKey("_sid");
    } else {
        joinKey = getjoinKey(key);
    }
    m_match->put(joinKey->mem, val);
    bs_delete(joinKey);
}

void MeJoinQuery::matchEqualTo(const char *key, int val) {
    if (!m_match) {
        m_match = new JSONObject();
    }
    
    string_t *joinKey = NULL;
    if (!strcmp("_id", key)) {
        joinKey = getjoinKey("_sid");
    } else {
        joinKey = getjoinKey(key);
    }
    m_match->put(joinKey->mem, val);
    bs_delete(joinKey);
}

void MeJoinQuery::matchGreater(const char *key, const char *val) {
    if (!m_match) {
        m_match = new JSONObject();
    }

    JSONObject obj;
    obj.put("$gt", val);
    string_t *joinKey = NULL;
    if (!strcmp("_id", key)) {
        joinKey = getjoinKey("_sid");
    } else {
        joinKey = getjoinKey(key);
    }
    m_match->put(joinKey->mem, &obj);
    bs_delete(joinKey);
}

void MeJoinQuery::matchGreater(const char *key, int val) {
    if (!m_match) {
        m_match = new JSONObject();
    }
    
    JSONObject obj;
    obj.put("$gt", val);
    string_t *joinKey = NULL;
    if (!strcmp("_id", key)) {
        joinKey = getjoinKey("_sid");
    } else {
        joinKey = getjoinKey(key);
    }
    m_match->put(joinKey->mem, &obj);
    bs_delete(joinKey);
}

void MeJoinQuery::matchLess(const char *key, const char *val) {
    if (!m_match) {
        m_match = new JSONObject();
    }

    JSONObject obj;
    obj.put("$lt", val);
    string_t *joinKey = NULL;
    if (!strcmp("_id", key)) {
        joinKey = getjoinKey("_sid");
    } else {
        joinKey = getjoinKey(key);
    }
    m_match->put(joinKey->mem, &obj);
    bs_delete(joinKey);
}

void MeJoinQuery::matchLess(const char *key, int val) {
    if (!m_match) {
        m_match = new JSONObject();
    }
    
    JSONObject obj;
    obj.put("$lt", val);
    string_t *joinKey = NULL;
    if (!strcmp("_id", key)) {
        joinKey = getjoinKey("_sid");
    } else {
        joinKey = getjoinKey(key);
    }
    m_match->put(joinKey->mem, &obj);
    bs_delete(joinKey);
}

void
MeJoinQuery::addForeignTable(const char *fromTable, const char *foreignKey, const char *localKey,
                             const char *document) {
    if (!m_lookup) {
        m_lookup = new JSONArray();
    }

    JSONObject *lookupObject = new JSONObject();
    lookupObject->setDestruct(false);
    lookupObject->put("from", fromTable);

    string_t *joinKey = NULL;
    if (!strcmp("_id", localKey)) {
        joinKey = getjoinKey("_sid");
    } else {
        joinKey = getjoinKey(localKey);
    }
    lookupObject->put("localField", joinKey->mem);
    bs_delete(joinKey);

    if (!strcmp("_id", foreignKey)) {
        lookupObject->put("foreignField", "_sid");
    } else {
        lookupObject->put("foreignField", foreignKey);
    }
    lookupObject->put("as", document);
    m_lookup->append(lookupObject);
    delete lookupObject;
}

void MeJoinQuery::addMeJoinQuery(MeJoinQuery *joinQuery) {
    if (joinQuery->m_nest_query) {
        m_child_querys.push_back(joinQuery);
    }
}

void MeJoinQuery::composeQueryArray(MeJoinQuery *query, JSONArray *jsonArray) {
    if (query->m_match) {
        JSONObject *match = new JSONObject();
        match->setDestruct(false);
        match->put("$match", query->m_match);
        jsonArray->append(match);
        delete match;
    }

    if (query->m_sort_keys) {
        JSONObject *sort = new JSONObject();
        sort->setDestruct(false);
        sort->put("$sort", query->m_sort_keys);
        jsonArray->append(sort);
        delete sort;
    }

    if (!query->m_nest_query && query->m_limit > ME_QUERY_DEFAULT_VALUE) {
        JSONObject *limit = new JSONObject();
        limit->setDestruct(false);
        limit->put("$limit", query->m_limit);
        jsonArray->append(limit);
        delete limit;
    }

    if (query->m_lookup) {
        for (int i = 0; i < query->m_lookup->size(); i++) {
            JSONObject *lookup = new JSONObject();
            lookup->setDestruct(false);
            JSONObject value = m_lookup->jsonValue(i);
            lookup->put("$lookup", &(value));
            jsonArray->append(lookup);
            delete lookup;
        }
    }

    if (query->m_select_keys) {
        JSONObject *select = new JSONObject();
        select->setDestruct(false);
        select->put("$project", query->m_select_keys);
        jsonArray->append(select);
        delete select;
    }
}

void MeJoinQuery::find(MeCallback *callback) {
    // 具有parent MeJoinQuery不支持关联查询
    if (m_nest_query) {
        return;
    }

    JSONArray *jsonArray = new JSONArray();
    composeQueryArray(this, jsonArray);

    if (m_child_querys.size() > 0) {
        vector<MeJoinQuery *> allChildQuery;
        MeJoinQuery *list = this;
        MeJoinQuery *current = this;
        while (current) {
            for (vector<MeJoinQuery *>::iterator iter = current->m_child_querys.begin();
                 iter != current->m_child_querys.end(); iter++) {
                list->next = (*iter);
                composeQueryArray(*iter, jsonArray);
                allChildQuery.push_back((*iter));
            }

            current = current->next;
        }
    }

    const char *whereKey = "aggregate";
    char *whereKeyOutput = MeCloud::shareInstance()->crypto(whereKey, (int) strlen(whereKey));
    char *whereParam = (char *) jsonArray->toString();
    char *whereParamOutput = MeCloud::shareInstance()->crypto(whereParam, (int) strlen(whereParam));
    snprintf(m_url, sizeof(m_url), "%s%s?%s=%s", MeCloud::shareInstance()->classUrl(), m_classname,
             whereKey, whereParamOutput);
    free(whereKeyOutput);
    free(whereParamOutput);

#if DEBUG
    int k = 0;
    for (int i = 0; i < strlen(m_url); ++i) {
        if (m_url[i] != '\n' && m_url[i] != '\t' && m_url[i] != ' ') {
            m_url[k++] = m_url[i];
        }
    }
    m_url[k] = '\0';
#else
#endif
    MeCloud::shareInstance()->get(m_url, callback);
    delete jsonArray;
}

MeJoinQuery::~MeJoinQuery() {
    delete m_match;
    m_match = NULL;
    delete m_select_keys;
    m_match = NULL;
    delete m_sort_keys;
    m_match = NULL;
    delete m_lookup;
    m_match = NULL;
    m_child_querys.clear();
}
