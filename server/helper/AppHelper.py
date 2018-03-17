#-*- coding: utf-8 -*- 
from ClassHelper import ClassHelper

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
			
