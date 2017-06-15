#-*- coding: utf-8 -*- 
import pymongo
import json
from bson import ObjectId
from datetime import *
from DbHelper import *
from Util import *
from model.MeError import *
from copy import *
from lib import *

class ClassHelper:
	# 不同数据库的class映射
	''' TODO 目前为使用一个map，应该在classHelper创建时实时查询 '''
	classMap = None
	### 构造函数，如果存在class映射，则使用映射class
	def __init__(self, className, appName=None):
		self.className = className;
		self.db = MongoDb(appName)
		self.appName = self.db.dbName()
		self.coll = className
		# class映射
		classmap = self.db.find_one("ClassMap", {"className": className})
		if classmap!=None:
			self.db = MongoDb(classmap['dbName'])
			self.coll = classmap['classMap']
			log.debug("use class map[%s/%s: %s/%s]" % (appName, className, classmap['dbName'], classmap['className']));

	def __del__(self):
		pass
	### 通过id获取对象
	def get(self, oid):
		return self.db.find_one(self.coll, {"_id": oid})
	### 数量
	def query_count(self, query):
		return self.db.query_count(self.coll, query)
	def count(self):
		return self.db.count(self.coll)	
	### 查询第一个对象	
	def find_one(self, query, keys=None):
		return self.db.find_one(self.coll, query, keys)
	
	### 查询
	def find(self, query, keys=None, sort=None, skip=0, limit=0):
		return self.db.find(self.coll, query, keys, sort=sort, skip=skip, limit=limit)

	### 创建，包含递归创建对象。
	def create(self, obj):
		t = self.create_core(obj)
		if t[1]:
			return dict(t[0], **t[1])
		else:
			return t[0]

	### 创建，包含递归创建对象。返回元组，第二个元素为递归对象
	def create_core(self, obj):
		log.debug('[%s] create object start: %s' % (self.className, obj))
		embed = None
		for key in obj.keys():
			# 如果是pointer类型，则递归处理对象
			if isinstance(obj[key], dict) and obj[key].has_key('_type') and obj[key]['_type']=='pointer' and obj[key].has_key('_content'):
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
				print "Key:"+key
				print "O: "+ str(o)
				print "embed1: " + str(embed)
				embed[key]['_id'] = o[0]['_id']
				# 将递归对象拼接起来
				if o[1]!=None:
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
		obj['_id'] = self.db.insert(self.coll, obj)
		return (obj, embed)


	### 更新，包含递归更新对象
	def update(self, oId, obj):
		t = self.update_core(oId, obj)
		if t[1]:
			return dict(t[0], **t[1])
		else:
			return t[0]

	### 更新，包含递归更新对象。返回元组，第二个元素为递归对象
	def update_core(self, oId, obj):
		log.debug('[%s/%s] update object start: %s' % (self.appName, self.className, obj))
		# 用于保存递归对象，递归对象内容不存到父对象
		embed = None

		if obj.has_key('$set'):
			for key in obj['$set'].keys():
				# 如果是pointer类型，则递归处理对象
				if isinstance(obj['$set'][key], dict) and obj['$set'][key].has_key('_type') and obj['$set'][key]['_type']=='pointer':
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

					print "Key:"+key
					print "O: "+ str(o)
					print "embed1: " + str(embed)
					embed[key]['_id'] = o[0]['_id']
					# 将递归对象拼接起来
					if o[1]!=None:
						if embed[key]['_content'].has_key('$set'):
							del embed[key]['_content']['$set']
						# 内部还有递归的情况，同级可能有多个递归对象
						for embed_key in o[1].keys():
							embed[key]['_content'][embed_key] = o[1][embed_key]
					else:
						embed[key]['_content'] = o[0]
					print "embed2: " + str(embed)
		else:
			obj['$set'] = {}
		obj['$set']['updateAt'] = datetime.now()
		try:
			return (self.db.update(self.coll, {"_id": oId}, obj), embed)
		except Exception, e:
			return (ERR_OBJECTID_MIS.message, {})

	def delete(self, oId=None, query=None):
		### modify by fangming.fm
		if query == None and oId == None:
			return false;
		if oId != None:
			self.db.remove(self.coll, {"_id": oId})
		else:
			self.db.remove(self.coll, query);

	### 设置索引
	def index(self, query):
		self.db.index(self.coll, query)
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
