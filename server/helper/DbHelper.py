# -*- coding: utf-8 -*-
'''
 * file :   DbHandler.py
 * author : bushaofeng
 * create : 2016-06-08 20:08
 * func : 数据库
 * history:
'''
import pymongo,json,copy,logging, pymysql
from bson import ObjectId
from bson import json_util
from datetime import datetime
from urllib import unquote
from mecloud.model.MeObject import *
from mecloud.model.MeError import *
from mecloud.lib import *
from Util import *

# 基类
class Db:
    # 数据库连接
    conn = None
    addr = None
    # 数据库名字
    name = None

    def __init__(self, db=None):
        if db:
            self.name = db
    @staticmethod
    def init(addr):
        Db.addr = addr
    @staticmethod
    def selectDb(dbName):
        Db.name = dbName

    def dbName(self):
        if self.name:
            return self.name
        else:
            return Db.name

    def find_one(self, collection, query):
        pass

    def find(self, collection, query):
        pass

    def insert(self, collection, obj):
        pass

    def remove(self, collection, query):
        pass

    def updateOne(self, collection, query, obj):
        pass

    def update(self, collection, query, obj):
        pass

## MongoDb封装
class MongoDb(Db):
    @staticmethod
    def connect(addr, port = None, replica_set=None, user=None, password=None):
        if isinstance(addr, str) or isinstance(addr, unicode):
            if not port:
                Db.conn = pymongo.MongoClient(addr)
            else:
                Db.conn = pymongo.MongoClient(addr, port)
        elif isinstance(addr, list) and replica_set!=None:
            Db.conn = pymongo.MongoClient(addr, replicaSet=replica_set)
        if user!=None and password!=None:
            Db.conn.admin.authenticate(user, password)

    ### 构造函数
    def __init__(self, dbName=None):
        if not Db.conn:
            Db.conn = pymongo.MongoClient(Db.addr)
        Db.__init__(self, dbName)
        self.db = Db.conn[self.dbName()]

    ### 查找第一个
    def find_one(self, collection, query, keys=None, sort=None):
        if sort==None:
            doc = self.db[collection].find_one(MongoDb.toBson(query), sort=[('_id', pymongo.DESCENDING)])
        else:
            doc = self.db[collection].find_one(MongoDb.toBson(query), sort = self.sortToTuple(sort))
        
        if not doc:
            return None

        return MongoDb.toJson(doc)

    ### 查询
    def find(self, collection, query, keys=None, sort=None, limit=8, skip=0):
        results = []
        if sort == None:
            items = self.db[collection].find(MongoDb.toBson(query), keys).sort('_id', pymongo.DESCENDING).skip(skip).limit(limit)
        else:
            items = self.db[collection].find(MongoDb.toBson(query), keys).sort(self.sortToTuple(sort)).skip(0).limit(limit)
        for item in items:
            results.append(MongoDb.toJson(item))
        return results
    
    """
    游标遍历
    """
    def cursor(self, collection, query, keys=None, sort=None, limit=10, skip=0):
        if sort == None:
            return self.db[collection].find(MongoDb.toBson(query), keys).sort('_id', -1).skip(skip).limit(limit)
        else:
            return self.db[collection].find(MongoDb.toBson(query), keys).sort(self.sortToTuple(sort)).skip(skip).limit(limit)
    """
    游标遍历
    """
    def next(self, cursor):
        try:
            obj = cursor.next()
            return MongoDb.toJson(obj)
        # except StopIteration,e:
        #     return None
        except Exception, __e:
            raise __e

    ### 管道查询查询
    def aggregate(self, collection, query):
        results = []
        items = self.db[collection].aggregate(self.aggregateJson(query))
        for item in items:
            results.append(MongoDb.toJson(item))
        return results

    def insert(self, collection, obj):
        return self.insert_one(collection, obj)
    
    '''
    单纯的新建操作
    '''
    def insert_one(self, collection, obj):
        if obj.has_key("_id"):
            obj["_sid"] = MongoDb.toId(obj['_id'])
            obj['_id'] = ObjectId(obj['_id'])

        try:
            obj['_id'] =  MongoDb.toId(self.db[collection].insert(MongoDb.toBson(obj)))
            if '_sid' not in obj:
                obj = self.update_one(collection, {'_id': obj['_id']}, {'$set':{'_sid': obj['_id']}})
            return obj
        except Exception,e:
            log.err('insert[%s] object[%s] error:%s', collection, MongoDb.toJson(obj), str(e))
            ###TODO: insert的不同错误抛出不同异常，如数据库挂掉、唯一冲突等
            if e.code==11000:
                err = copy.deepcopy(ERR_OBJECT_DUP)
                err.message['info'] = e.message
                raise err
            return None

    ### 删除
    def remove(self, collection, query):
        result =  self.db[collection].remove(MongoDb.toBson(query))
        if result['n'] == 0:
            log.err('remove[%s: %s] not found', collection, json.dumps(query))
            e = copy.deepcopy(ERR_NOTFOUND)
            e.message['errMsg'] = "remove[%s: %s] not found"%(collection, json.dumps(query))
            raise e
        return result

    """
    @ upsert: 没有是否新建
    @ new: 是否返回更新后的值, 如果upsert为True, 且之前没有数据，则返回新创建的数据的id
    """
    def update_one(self, collection, query, obj, upsert=False, new=True):
        if new:
            return_doc = pymongo.ReturnDocument.AFTER
        else:
            return_doc = pymongo.ReturnDocument.BEFORE

        doc = self.db[collection].find_one_and_update(MongoDb.toBson(query), obj, upsert=upsert, return_document= return_doc)
        if not doc:
            # 如果upsert为True， 则返回新创建数据的id
            if upsert:
                doc = self.find_one(collection, query)
                if 'createAt' not in doc:
                    doc = self.update_one(collection,{"_id":doc['_id']}, {'$set': {'createAt': doc['updateAt'], "_sid": str(doc['_id'])}})
                if doc:
                    return {'_id': doc['_id']}
            log.err('update[%s: %s] not found', collection, json.dumps(query))
            return None

        return MongoDb.toJson(doc)

    ### 数量
    def query_count(self, collection, query):
        return self.db[collection].find(MongoDb.toBson(query)).count()

    def count(self, collection):
        return self.db[collection].count()

    ### 去重
    def distinct(self, collection, query=None, field=None):
        return self.db[collection].distinct(field, MongoDb.toBson(query))

    ### 更新many
    def update_many(self, collection, query, obj):
        doc = self.db[collection].update(MongoDb.toBson(query), obj, multi=True)
        return MongoDb.toJson(doc)

    ### 设置索引, eg. [("date", DESCENDING), ("author", ASCENDING)]
    def index(self, collection, query, **kwargs):
        self.db[collection].create_index(query, **kwargs);
    def listIndex(self, collection):
        result = []
        items = self.db[collection].list_indexes()
        for item in items:
            result.append(item['key'])
        return result

    def dropIndex(self, collection, query):
        self.db[collection].drop_index(query)
    ### 唯一索引
    def unique(self, collection, key):
        self.db[collection].ensure_index(key, unique=True)

    ### ObjectId转换为字符串
    @staticmethod
    def toId(oid):
        if isinstance(oid, (str, unicode)):
            return oid
        id = eval(json_util.dumps(oid))
        return id['$oid']

    ### 字符串转换为ObjectId
    @staticmethod
    def toOId(id):
        oid = json_util.loads({'$oid': id})
        return oid

    ### 将Bson转换为普通json
    @staticmethod
    def toJson(obj):
        if obj.has_key('_id') and obj['_id']:
            obj['_id'] = MongoDb.toId(obj['_id'])
        for key in obj.iterkeys():
            if type(obj[key]) == list:
                for item in obj[key]:
                    try:
                        if item.has_key('_id') and item['_id']:
                            item['_id'] = MongoDb.toId(item['_id'])
                    except Exception:
                        continue
        return obj

    ### 将json转换为mongodb可用的bson
    @staticmethod
    def toBson(obj):
        ##modify by fangming.fm
        if obj.has_key('_id'):
            ##单id查询
            if isinstance(obj['_id'], ObjectId):
                pass
            elif isinstance(obj['_id'], str) or isinstance(obj['_id'], unicode):
                obj['_id'] = ObjectId(obj['_id'])
            ##id支持in条件查询
            elif obj['_id'].has_key('$in'):
                inobj = [];
                for _id in obj['_id']['$in']:
                    inobj.append(ObjectId(_id));
                obj['_id'] = {'$in': inobj};
                return obj;
            else:
                for k in obj['_id']:
                    obj['_id'][k] = ObjectId(obj['_id'][k])
                return obj;

        #obj = {'updateAt':{'$gt':'2017-01-01 00:00:00'}}
        if obj.has_key("updateAt") and isinstance(obj['updateAt'],dict):
            item = obj['updateAt']
            for key, value in item.items():
                if isinstance(value, str) or isinstance(value, unicode):
                    try:
                        item[key] = datetime.strptime(value, "%Y-%m-%d %H:%M:%S.%f")
                    except:
                        item[key] = datetime.strptime(value.split('.')[0], "%Y-%m-%d %H:%M:%S")
            obj['updateAt'] = item

        if obj.has_key("createAt") and isinstance(obj['createAt'],dict):
            item = obj['createAt']
            for key, value in item.items():
                if isinstance(value, str) or isinstance(value, unicode):
                    try:
                        item[key] = datetime.strptime(value, "%Y-%m-%d %H:%M:%S.%f")
                    except:
                        item[key] = datetime.strptime(value.split('.')[0], "%Y-%m-%d %H:%M:%S")
            obj['createAt'] = item
        if obj.has_key("shotTime"):
            obj['shotTime'] = datetime.strptime(obj['shotTime'].split('.')[0], "%Y-%m-%d %H:%M:%S")
        return obj

    @staticmethod
    def sortToTuple(obj):
        sort = []
        for key, value in obj.items():
            item = (key, value)
            sort.append(item)
        return sort

    @staticmethod
    def aggregateJson(obj):
        '''
        :param obj: 
        :return: 
        '''
        for item in obj:
            match = item.get("$match")
            if match:
                updateAt = match.get("updateAt")
                if updateAt:
                    for key, value in updateAt.items():
                        match['updateAt'][key] = datetime.strptime(unquote(value).split('.')[0], "%Y-%m-%d %H:%M:%S")
                createAt = match.get("createAt")
                if createAt:
                    for key, value in createAt.items():
                        match['createAt'][key] = datetime.strptime(unquote(value).split('.')[0], "%Y-%m-%d %H:%M:%S")
        return obj

    def parse(self, obj):
        new_obj = {}
        for (k,v) in obj.items():
            if isinstance(v, dict):
                v = self.parse(v)
            if k=='@inc':
                new_obj['$inc'] = v
            elif k=='@set':
                new_obj['$set'] = v
            elif k=='@push':
                new_obj['$push'] = v
            elif k=='@pull':
                new_obj['$pull'] = v
            else:
                new_obj[k] = v
        return new_obj

## MySql封装
class MySqlDb(Db):
    def __init__(self, dbName=None):
        Db.__init__(self, dbName)
        Db.conn.select_db(self.dbName())
        self.db = Db.conn
        self.cursor = Db.conn.cursor()

    @staticmethod
    def connect(addr, port = None, user=None, password=None):
        if not port:
            port = 3306
        Db.conn = pymysql.connect(host=addr, port=port, user=user,
                                  password=password, charset='utf8mb4',
                                  cursorclass=pymysql.cursors.DictCursor)

    """
        查询一条
    """
    def find_one(self, collection, query, keys="*"):
        try:
            sql = "select {0} from {1}".format(keys,collection)
            if not query:
                sql += " where {0}".format(self.dict2str(query,","))
            self.cursor.execute(sql)
            result = self.cursor.fetchone()
            print result
        except Exception,e:
            print e
            log.err("find_one err:%s", e)
    def find(self, collection, query, keys="*", sort=None, limit=8, skip=0):
        try:
            sql = "select {0} from {1}".format(keys, collection)
            if not query:
                sql += " where {0} limit {1},{2}".format(self.dict2str(query,","), skip, limit)
            self.cursor.execute(sql)
            result = self.cursor.fetchmany(limit)
            print result
        except Exception,e:

            log.err("find err:%s",e)

    def updateOne(self, collection, query, obj):
        try:
            if not obj.has_key("update_date"):
                obj["update_date"] = "now()"
            sql = "update {0} set {1} where {2}".format(collection, self.dict2str(obj,","), self.dict2str(query,","))
            result = self.cursor.execute(sql)
            self.db.commit()
            print result
        except Exception,e:
            log.err("updateOne err:%s",e)

    def insertOne(self, collection, obj):
        try:
            if not obj.has_key('create_date'):
                obj["create_date"] = "now()"
                obj["update_date"] = obj["create_date"]
            sql = "insert into {0} set {1}".format(collection,self.dict2str(obj,","))
            result = self.cursor.execute(sql)
            self.db.commit()
            print result
        except Exception,e:
            self.db.rollback()
            log.err("insert err:%s",e)

    def safe(self,s):
        return pymysql.escape_string(s)
    """
        将json串的key和value转化为字符串
    """
    def dict2str(self, dictin, joinString):
        '''
            将字典变成，key='value',key='value' 的形式
            '''
        tmplist = []
        for k, v in dictin.items():
            if v == "now()":  # 当前时间
                tmp = "%s=%s" % (str(k), str(v))
            elif type(v) is str:
                tmp = "%s='%s'" % (str(k), str(v))
            else:
                tmp = "%s=%s" % (str(k), str(v))
            tmplist.append(' ' + tmp + ' ')
        return joinString.join(tmplist)
    # """
    #     将json串的key和value分别转化为字符串
    # """
    # def dictToString(self,obj,joinString):
    #     return joinString.join(obj.keys()),joinString.join([self.convert(x) for x in obj.values()])

    """
    转化为字符串，字符串添加单引号
    """
    # def convert(self,value):
    #     if value == "now()":#当前时间
    #         return value
    #     if type(value) is str:
    #         return "'" + value + "'"
    #     else:
    #         return str(value)

    def close(self):
        self.cursor.close()
        Db.conn.close()




## SqlServer封装
class SqlServerDb(Db):
    def __init__(self, dbName):
        Db.__init__(self, dbName)
