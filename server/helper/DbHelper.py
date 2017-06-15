#-*- coding: utf-8 -*- 
import pymongo
import json
from bson import ObjectId
from bson import json_util

# 基类
class Db:
	conn = None;
	name = None
	def __init__(self, db=None):
		if db:
			self.name = db
	def dbName(self):
		if self.name:
			return self.name
		else:
			return name

	def find_one(self, collection, query):
		pass
	def find(self, collection, query):
		pass
	def insert(self, collection, obj):
		pass
	def remove(self, collection, query):
		pass
	def update(self, collection, query, obj):
		pass
## MongoDb封装
class MongoDb(Db):
	### 构造函数
	def __init__(self, dbName=None):
		Db.__init__(self, dbName)
		self.db = Db.conn[self.dbName()]

	### 查找第一个
	def find_one(self, collection, query, keys=None):
		doc = self.db[collection].find_one(MongoDb.toBson(query), keys);
		if not doc:
			return None
		return MongoDb.toJson(doc)
	### 数量
	def query_count(self, collection, query):
		return self.db[collection].find(MongoDb.toBson(query)).count()
	def count(self, collection):
		return self.db[collection].count()	
	### 查询
	def find(self, collection, query, keys=None, sort=None, skip=0, limit=8):
		results =[]
		if sort == None:
			items = self.db[collection].find(MongoDb.toBson(query), keys).sort('_id',-1).skip(skip).limit(limit)
		else:
			items = self.db[collection].find(MongoDb.toBson(query), keys).sort('_id', sort).skip(skip).limit(limit)
		for item in items:
			results.append(MongoDb.toJson(item))
		return results;

	### 插入
	def insert(self, collection, obj):
		return MongoDb.toId(self.db[collection].insert(obj))
	### 删除
	def remove(self, collection, query):
		return self.db[collection].remove(MongoDb.toBson(query))
	### 更新，返回更新后的整个对象
	def update(self, collection, query, obj):
		doc = self.db[collection].find_and_modify(MongoDb.toBson(query), obj, upsert=True, new=True);
		return MongoDb.toJson(doc)

	### 设置索引, eg. [("date", DESCENDING), ("author", ASCENDING)]
	def index(self, collection, query):
		self.db[collection].create_index(query);

	### 唯一索引
	def unique(self, collection, key):
		self.db[collection].ensure_index(key, unique=True)

	### ObjectId转换为字符串
	@staticmethod
	def toId(oid):
		id = eval(json_util.dumps(oid))
		return id['$oid']
	### 字符串转换为ObjectId
	@staticmethod
	def toOId(id):
		oid = json_util.loads({'$oid':id})
		return oid
	### 将Bson转换为普通json
	@staticmethod
	def toJson(obj):
		if obj['_id']:
			obj['_id'] = MongoDb.toId(obj['_id'])
		return obj
	### 将json转换为mongodb可用的bson
	@staticmethod
	def toBson(obj):
		##modify by fangming.fm
		if obj.has_key('_id'):
			##单id查询		
			if isinstance(obj['_id'], str) or isinstance(obj['_id'], unicode):
				obj['_id'] = ObjectId(obj['_id'])
			##id支持in条件查询
			elif obj['_id'].has_key('$in'):
				inobj = [];
				for _id in obj['_id']['$in']:
					inobj.append(ObjectId(_id));
				obj['_id'] = {'$in': inobj};
				return obj;
			else:
				return None;
		return obj;
		 	

## MySql封装
class MySqlDb(Db):
	def __init__(self, dbName):
		Db.__init__(self, dbName)

## SqlServer封装
class SqlServerDb(Db):
	def __init__(self, dbName):
		Db.__init__(self, dbName)
