# -*- coding: utf-8 -*-
import json

import elasticsearch

import EsClient

ES_SERVERS = [{
    'host': 't02.me-yun.com',
    'port': 9200
}]

es = None

USER_INDEX = 'user_index'
USER_DOC_TYPE = 'user'
USER_SCROLL = '3m'
# 默认每页显示数据
USER_PAGE_SIZE = 20


def init():
    EsClient.es = elasticsearch.Elasticsearch(
        hosts=ES_SERVERS
    )


# 根据条件直接搜索
def search(indexName, docType, query):
    searched = es.search(index=indexName, doc_type=docType, body=query, scroll='3m')
    print searched['_scroll_id']
    json_str = json.dumps(searched, ensure_ascii=False)
    print json_str
    return searched


# 根据scrollId获取数据
def scroll(scrollId, scrollExistTime):
    searched = es.scroll(scroll_id=scrollId, scroll=scrollExistTime)
    print json.dumps(searched, ensure_ascii=False)
    return searched


# 搜索user
def searchUser(content, scrollId, pageSize):
    if scrollId:
        return scroll(scrollId, USER_SCROLL)
    elif content:
        query = buildUserSearchQuery(content, pageSize)
        return search(USER_INDEX, USER_DOC_TYPE, query)


# user搜索query
def buildUserSearchQuery(content, pageSize):
    # query = {
    #     'query': {
    #         'multi_match': {
    #             "query": content,
    #             "fields": ["nickName", "pinyin", "pinyinsmall"]
    #             , "fuzziness": "AUTO"
    #         }
    #     },
    #     'size': pageSize,
    #     "sort": [
    #         {"_score": {"order": "desc"}}
    #     ]
    # }

    query = {
        "query": {
            "bool": {
                "should": [
                    {"wildcard": {"nickName": "*" + content + "*"}},
                    {"wildcard": {"nickName": content + "*"}},
                    {"wildcard": {"pinyin": "*" + content + "*"}},
                    {"wildcard": {"pinyinsmall": "*" + content + "*"}},
                    {"bool": {
                        "should": [
                            {"match": {"nickName": content}},
                            {"match": {"pinyin": content}},
                            {"match": {"pinyinsmall": content}}
                        ]
                        # ,
                        # "must": [{"wildcard": {"nickName": "*" + content + "*"}},
                        #          {"wildcard": {"nickName": content + "*"}}]
                    }}
                ]
            }
        },
        'size': pageSize
    }
    # query = {
    #     "query": {
    #         "bool": {
    #             "should": [
    #                 {"match": {"nickName": content}},
    #                 {"match": {"pinyin": content}},
    #                 {"match": {"pinyinsmall": content}}
    #             ]
    #
    #         }
    #     }
    # }
    return query

# 查询所有记录 测试用
# def buildUserSearchQuery(content, pageSize):
#     query = {
#         'query': {
#             'match_all': {}
#         },
#         'size': pageSize
#     }
#     return query
