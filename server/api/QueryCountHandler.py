# -*- coding: utf-8 -*-

import json
import copy
import tornado.web
from mecloud.api.ClassHandler import ClassHandler
from bson import ObjectId
from mecloud.helper.ClassHelper import ClassHelper
from mecloud.helper.Util import MeEncoder
from mecloud.lib import log
from mecloud.model.MeACL import MeACL
from mecloud.model.MeError import *
from mecloud.model.MeObject import MeObject
from mecloud.api.BaseHandler import BaseConfig


class QueryCountHandler(ClassHandler):
    ### 获取对象 及 批量查询
    @tornado.web.authenticated
    def get(self,action=None):
        '''
        # where 条件格式：[{
                            "condition": {
                                "user": "xxx"
                            },
                            "key": "xxx",
                            "method": "count/id/list/jsonObject",
                            "classname": "xxx"
                        },
                        {
                            "condition": {
                                "user": "xxx"
                            },
                            "key": "xxx",
                            "method": "count/id/list/jsonObject",
                            "classname": "xxx"
                        }]
        #返回数据格式：{"a":11,"b":10}
        '''
        if action == "albumConfig":
            if self.request.arguments.has_key('where'):
                query = eval(self.get_argument('where'))
                query['settingId'] = 6
                query['version'] = 0
                classHelper = ClassHelper("VersionControl")
                objs = classHelper.find_one(query,keys={'albumConfig':1})
                albumConfig = objs.get('albumConfig',None)
                if albumConfig:
                    albumConfig.sort(key=lambda k: (k['weight']), reverse=False)
                    items = []
                    classHelper = ClassHelper("StatCount")
                    for i,cfig in enumerate(albumConfig):
                        count = classHelper.find_one({'name':cfig['type']+ "_" +self.user['_id']}) or {}
                        count = count.get('count',0)
                        item = {
                            'title':cfig['title'],
                            'authorize':cfig['authorize'],
                            'type':cfig['type'],
                            'count':count
                        }
                        items.append(item)
                        if i >= 5:
                            break
                    data = copy.deepcopy(ERR_SUCCESS)
                    data.message['data'] = items
                    self.write(json.dumps(data.message,cls=MeEncoder))
                else:
                    self.write(ERR_PATH_PERMISSION.message)

            else:
                self.write(ERR_PATH_PERMISSION.message)

            # self.write("this is myTest")
        elif self.request.arguments.has_key('where'):
            where = eval(self.get_argument('where'))
            result = {}
            for item in where:
                className = item['classname']
                if self.user['_id'] not in BaseConfig.adminUser:
                    if className == 'User':
                        continue
                query = item["condition"]
                try:
                    if query.has_key('_id'):
                        ObjectId(query['_id'])
                        # if query.has_key('$or'):
                        #     for item in query['$or']:
                        #         if "_id" in item:
                        #             item["_id"] = ObjectId(item["_id"])
                except Exception:
                    self.write(ERR_OBJECTID_MIS.message)
                    return
                try:
                    if item['method'] == 1:
                        result[item['key']] = self.getCount(className, query)
                    elif item['method'] == 2:
                        result[item['key']] = self.getId(className, query)
                    elif item['method'] == 3:
                        result[item['key']] = self.getList(className, query)
                    elif item['method'] == 4:
                        result[item['key']] = self.getJson(className, query)
                    elif item['method'] == 5:
                        result[item['key']] = self.getDistinct(className, query, item['distinct'])
                except Exception,e:
                    log.err("%s param error", item['key'])
            self.write(json.dumps(result,cls=MeEncoder))
        else:
            self.write(ERR_PARA.message)
    def getCount(self, className, query):
        classHelper = ClassHelper(className)
        result = classHelper.query_count(query)
        return result


    def getId(self, className, query):
        classHelper = ClassHelper(className)
        result = classHelper.find_one(query,keys={"_id":1,"acl":1})
        acl = MeACL(result['acl'])
        if not acl.readAccess(self.user):
            return None
        return result['_id']

    def getList(self, className, query):
        classHelper = ClassHelper(className)
        result = classHelper.find(query)
        objects = []
        for obj in result:
            mo = MeObject(className, obj, False)
            mo.overLoadGet = False
            acl = MeACL(mo['acl'])
            if not acl.readAccess(self.user):
                continue
            objects.append(mo)
        if len(objects) == 0:
            return None
        return objects

    def getJson(self, className, query):
        classHelper = ClassHelper(className)
        result = classHelper.find_one(query)
        mo = MeObject(className, result, False)
        mo.overLoadGet = False
        acl = MeACL(mo['acl'])
        if not acl.readAccess(self.user):
            return None
        return mo

    def getDistinct(self, className, query, field):
        classHelper = ClassHelper(className)
        result = classHelper.distinct(query, field)
        return len(result.cursor)



        #where 条件格式：[{"classname":{"name":"11"}},{"classname":{"name":"11"}}]
        #返回数据格式：{"a":11,"b":10}
        # if self.request.arguments.has_key('where'):
        #     where = eval(self.get_argument('where'))
        #     counts = {}
        #     for item in where:
        #         for className in item.iterkeys():
        #             if className == 'User':
        #                 continue
        #             classHelper = ClassHelper(className)
        #             query = item[className]
        #             try:
        #                 if query.has_key('_id'):
        #                     ObjectId(query['_id'])
        #                 if query.has_key('$or'):
        #                     for item in query['$or']:
        #                         if "_id" in item:
        #                             item["_id"] = ObjectId(item["_id"])
        #             except Exception:
        #                 self.write(ERR_OBJECTID_MIS.message)
        #                 return
        #             count = classHelper.query_count(query)
        #             counts[className] = count
        #     self.write(json.dumps(counts));

        # else:
        #     self.write(ERR_PARA.message)

