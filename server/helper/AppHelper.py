#-*- coding: utf-8 -*- 
import pymongo
import json
from bson import ObjectId
from bson import json_util
from ClassHelper import *
from DbHelper import *

class AppHelper(dict):
	@staticmethod
	def get(appId, appKey):
		classHelper = ClassHelper('develop', 'App')
		return classHelper.find_one({'appId':appId, 'appKey':appKey})

	@staticmethod
	def getDbName(appId, appKey):
		app = AppHelper.get(appId, appKey);
		if not app:
			return None;
		return app['appName']

	@staticmethod
	def addApp(appName):
		pass
			
