# -*- coding: utf-8 -*-
import json
from datetime import datetime
from copy import deepcopy
from mecloud.helper.DbHelper import MongoDb
from mecloud.helper.Util import MeEncoder
from mecloud.lib import log
from mecloud.model.MeError import *
from Util import *

class ClassHelper:
    # 不同数据库的class映射
    ''' TODO 目前为使用一个map，应该在classHelper创建时实时查询 '''
    classMap = None
    is_online=False
    ### 构造函数，如果存在class映射，则使用映射class
    def __init__(self, className, appName=None):
        self.className = className
        self.db = MongoDb(appName)
        self.appName = self.db.dbName()
        self.coll = className

    def __del__(self):
        pass

    ### 通过id获取对象
    def get(self, oid, keys=None):
        obj = self.db.find_one(self.coll, {"_id": oid}, keys)
        return obj
    ### 查询第一个对象
    def find_one(self, query, keys=None, sort=None):
        return self.db.find_one(self.coll, query, keys, sort)

    ### 查询第一个对象如果没找到则创建一个对象并返回
    def find_and_create(self, query, obj=None, keys=None):
        item =  self.db.find_one(self.coll, query, keys)
        if not item:
            return self.create(obj)
        else:
            return item
    ### 如果有则更新，如果没有则创建
    def update_and_create(self, query, obj):
        if 'updateAt' not in obj:
            obj['updateAt'] = datetime.now()
        if "$set" not in obj:
            obj = {'$set':obj}
        obj = self.db.update_one(self.coll, query=query, obj=obj,upsert=True,new=True)
        # 没有createAt说明是新建
        if 'createAt' not in obj:
            obj = self.db.update_one(self.coll, {'_id': obj['_id']}, {'$set':{'createAt':datetime.now(),"_sid":str(obj['_id'])}})
        return obj
    # ### 查询
    # def find(self, query, keys=None, sort=None,limit=0):
    #     return self.db.find(self.coll, query, keys, sort=sort,limit=limit)
    """
    查询，返回一个游标
    """
    def find(self, query, keys=None, sort=None, limit=0, skip=0, cache_next_page=False):
        self.cursor = self.db.cursor( self.coll, query, keys, sort, limit, skip )
        return self

    # 删除数据
    def remove(self, query):
        self.db.remove(self.coll, query)
        return

    """
    游标遍历迭代器，注意同一个对象不能同时进行两个查询的迭代
    """
    def __iter__(self):
        return self
    def next(self):
        return self.db.next(self.cursor)
    __next__ = next
    """
    关闭游标
    """
    def close(self):
        if self.cursor:
            self.cursor.close()

    ### 数量
    def query_count(self, query):
        return self.db.query_count(self.coll, query)

    def count(self):
        return self.db.count(self.coll)

    ### 去重
    def distinct(self, query, field):
        self.cursor = self.db.distinct(self.coll, query, field)
        return self

    ###使用skip查询
    # @deprecated
    # def find_use_skip(self, query, keys=None, sort=None, skip=0, limit=0):
    #     log.warn("If skip is large, speed is slow! Please don't use find_use_skip!")
    #     return self.db.find_use_skip(self.coll, query, keys, sort=sort, skip=skip, limit=limit)

    ### 查询
    def aggregate(self, query):
        return self.db.aggregate(self.coll, query)

    """
    创建对象
    @ obj: 对象内容
    """
    def create(self, obj):
        if not obj.has_key('updateAt'):
            obj['updateAt'] = datetime.now()
        if not obj.has_key('createAt'):
            obj['createAt'] = datetime.now()
        obj = self.db.insert(self.coll, obj)
        return obj
    
    ### 更新，包含递归更新对象
    @deprecated 
    def update(self, oId, obj, updateAt=True):
        log.warn("update is deprecated, please use updateWithId!")
        if "$set" not in obj:
            obj['$set'] = {}
        if updateAt:
            obj['$set']['updateAt'] = datetime.now()
        result = self.db.update_one(self.coll, {"_id": oId}, obj)
        return result
    
    """
    根据Id来更新文档
    """
    def update_with_id(self, oId, obj):
        if "$set" not in obj:
            obj['$set'] = {}
        obj['$set']['updateAt'] = datetime.now()
        result = self.db.update_one(self.coll, {"_id": oId}, obj)
        return result

    """
    更新第一条数据
    """
    def update_one(self, query, obj, upsert=False):
        if "$set" not in obj:
            obj['$set'] = {}
        if not obj['$set'].has_key("updateAt"):
            obj['$set']['updateAt'] = datetime.now()
        result = self.db.update_one(self.coll, query, obj, upsert)
        return result
    
    """
    批量更新
    """
    def update_many(self, query, obj):
        return self.db.update_many(self.coll, query, obj)

    """
    删除数据
    """
    def delete(self, oId=None, query=None):
        ### modify by fangming.fm
        if query == None and oId == None:
            return False
        if oId != None:
            self.db.remove(self.coll, {"_id": oId})
        else:
            self.db.remove(self.coll, query)

    ### 设置索引
    def index(self, query, **kwargs):
        self.db.index(self.coll, query, **kwargs)

    ### 设置唯一索引
    def unique(self, key):
        self.db.unique(self.coll, key)

    @staticmethod
    def createWithoutData(obj):
        o = {}
        o['_id'] = obj['_id']
        o['updateAt'] = obj['updateAt']
        o['createAt'] = obj['createAt']
        return o

    ### 创建，包含递归创建对象。返回元组，第二个元素为递归对象
    def create_core(self, obj):
        log.debug('[%s] create object start: %s' % (self.className, obj))
        embed = None
        for key in obj.keys():
            # 如果是pointer类型，则递归处理对象
            if isinstance(obj[key], dict) and obj[key].has_key('_type') and obj[key]['_type'] == 'pointer' and obj[
                key].has_key('_content'):
                # 创建对应的classhelper
                c = ClassHelper(obj[key]['_class'], self.appName)
                # 用于保存递归对象，递归对象内容不存到父对象
                embed = {}
                embed[key] = deepcopy(obj[key])
                # 如果存在_id，则递归update
                if obj[key].has_key('_id'):
                    o = c.update_core(obj[key]['_id'], obj[key]['_content'])
                else:
                    o = c.create_core(obj[key]['_content'])
                # 删除_content，使得父对象存储pointer对象
                ''' TODO 此处无法保存updateAt和createAt返回客户端 '''
                del obj[key]['_content']

                obj[key]['_id'] = o[0]['_id'];
                print "Key:" + key
                print "O: " + str(o)
                print "embed1: " + str(embed)
                embed[key]['_id'] = o[0]['_id']
                # 将递归对象拼接起来
                if o[1] != None:
                    if embed[key]['_content'].has_key('$set'):
                        del embed[key]['_content']['$set']
                    # 内部还有递归的情况，同级可能有多个递归对象
                    for embed_key in o[1].keys():
                        embed[key]['_content'][embed_key] = o[1][embed_key]
                else:
                    embed[key]['_content'] = o[0]
                print "embed2: " + str(embed)
        obj['updateAt'] = datetime.now()
        obj['createAt'] = obj['updateAt']
        try:
            if obj.has_key("_id"):
                obj["_sid"] = MongoDb.toId(obj['_id'])
        except Exception, e:
            log.err("Error:%s", str(e))
        obj['_id'] = self.db.insert(self.coll, obj)
        return (obj, embed)

    ### 更新，包含递归更新对象。返回元组，第二个元素为递归对象
    def update_core(self, oId, obj):
        log.debug('[%s/%s] update object start: %s' % (self.appName, self.className, obj))
        # 用于保存递归对象，递归对象内容不存到父对象
        embed = None

        if obj.has_key('$set'):
            for key in obj['$set'].keys():
                # 如果是pointer类型，则递归处理对象
                if isinstance(obj['$set'][key], dict) and obj['$set'][key].has_key('_type') and obj['$set'][key][
                    '_type'] == 'pointer':
                    # 创建对应的classhelper
                    c = ClassHelper(obj['$set'][key]['_class'], self.appName)
                    embed = {}
                    embed[key] = deepcopy(obj['$set'][key])
                    # 如果存在_id，则递归update
                    if obj['$set'][key].has_key('_id'):
                        o = c.update_core(obj['$set'][key]['_id'], obj['$set'][key]['_content'])
                    else:
                        o = c.create_core(obj['$set'][key]['_content'])
                    # 删除_content，使得父对象存储pointer对象
                    ''' TODO 此处无法保存updateAt和createAt返回客户端 '''
                    del obj['$set'][key]['_content']
                    obj['$set'][key]['_id'] = o[0]['_id'];

                    print "Key:" + key
                    print "O: " + str(o)
                    print "embed1: " + str(embed)
                    embed[key]['_id'] = o[0]['_id']
                    # 将递归对象拼接起来
                    if o[1] != None:
                        if embed[key]['_content'].has_key('$set'):
                            del embed[key]['_content']['$set']
                        # 内部还有递归的情况，同级可能有多个递归对象c
                        for embed_key in o[1].keys():
                            embed[key]['_content'][embed_key] = o[1][embed_key]
                    else:
                        embed[key]['_content'] = o[0]
                    print "embed2: " + str(embed)
        else:
            obj['$set'] = {}
        obj['$set']['updateAt'] = datetime.now()
        try:
            result = (self.db.update(self.coll, {"_id": oId}, obj), embed)
            return result
        except Exception, e:
            return (ERR_OBJECTID_MIS.message, {})

    '''
    def collection(self, className):
        if not self.db[className]:
            classMap = self.db.classMap.find_one({'class': className});
            db = self.db.conn[classMap['dbname']];
            #if classMap.has_key('username'):
            #	db.authenticate(classMap['username'], classMap['passwd']);
            return db[className]
        else:
            return self.db[className]
    '''
