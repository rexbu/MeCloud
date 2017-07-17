# coding=utf8
import tornado.web
from bson import json_util
import json
from datetime import *
from BaseHandler import *
from model.DevelopUser import *
from model.MeError import *
from helper.DbHelper import *
from helper.ClassHelper import *
from helper.Util import *
import copy


class UserHandler(BaseHandler):
	@tornado.web.authenticated
	def get(self):
		user = DevelopUser()
		user.get(self.get_current_user())
		if user==None:
			self.write(ERR_OBJECTID_MIS.message)
		del(user['password'])
		acl = MeACL(user['acl'])
		if acl.readAccess(user):
			self.write(user)
		else:
			self.write(ERR_USER_PERMISSION.message)

	def post(self, action=None):
		if action=='signup':
			print "Signup"
			self.signup()
		elif action=='login':
			self.login()
		elif action=='modifyPwd':
			pass
		elif action=='update':
			pass
		elif action=='loginWithoutPwd':
			self.loginWithoutPwd()
		else:
			print "action error: "+action

	### 注册接口
	def signup(self):
		user = MeUser(self.jsonBody)
		if user['username']==None or user['password']==None:
			self.write(ERR_PARA.message)
			return
		try:
			user.signup()
			self.write(json.dumps(user, cls=MeEncoder))
		except Exception,e:
			#TODO: 暂时只有重复
			self.write(ERR_USER_TAKEN.message)

	def login(self):
		user = MeUser(self.jsonBody)
		if user['username']==None or user['password']==None:
			self.write(ERR_PARA.message)
			return
		if user.login(user['username'], user['password']):
			self.write(json.dumps(user, cls=MeEncoder))
		else:
			self.write(ERR_USERPWD_MISMATCH.message)
			
	### 登录接口
	def login(self):
		user = MeUser(self.jsonBody)
		if user['username']==None or user['password']==None:
			self.write(ERR_PARA.message)

		if user.login(user['username'], user['password']):
			self.write(json.dumps(user, cls=MeEncoder))
		else:
			self.write(ERR_USERPWD_MISMATCH.message)

	def loginWithoutPwd(self):
		obj = json.loads(self.request.body)
		if not checkKeys(obj, ['username']):
			self.write(ERR_PARA.message)
			return

		user = MeUser(self.appName, obj)
		userHelper = ClassHelper('develop', 'User')
		userInfo = userHelper.get(self.appInfo['user'])
		# library授权
		if userInfo['type']==2:
			if user['bundleId']:
				log.info("Library User[%s] Auth. bundleId[%s]", user['username'], user['bundleId']);
			elif user['package']:
				log.info("Library User[%s] Auth. package[%s]", user['username'], user['package']);
			log.info('auth app[%s]', self.appInfo['appName']);
		# 普通授权失败
		elif user['bundleId'] != None:
			if (not self.appInfo.has_key('bundleId')) or self.appInfo['bundleId']!= user['bundleId']:
				log.err('[%s] bundleId[%s] not match. LoginWithoutPwd Error.', self.appInfo['appName'], user['bundleId'])
				self.write(ERR_UNAUTHORIZED.message)
				return
			log.info('auth app[%s]', self.appInfo['appName']);
		elif user['package']:
			if (not self.appInfo.has_key('package')) or self.appInfo['package'] != user['package']:
				log.err('[%s] package[%s] not match. LoginWithoutPwd Error.', self.appInfo['appName'], user['package'])
				self.write(ERR_UNAUTHORIZED.message)
				return
			log.info('auth app[%s]', self.appInfo['appName']);
		else:
			log.err("loginWithoutPwd Error: Invalid. %s", self.request.body)
			self.write(ERR_UNAUTHORIZED.message)
			return
		
		# 检查数量限制
		# userHelper = ClassHelper(self.appDb, "User")
		# userUpper = self.appInfo['userUpper']
		# # userUpper=0表示无数量限制
		# if userUpper>0:
		# 	if userHelper.count() > userUpper:
		# 		log.err('[%s] over user count limit', self.appInfo['appName']);
		# 		self.write(ERR_USER_PERMISSION.message)

		try:
			user.loginWithoutPwd()
			log.info('LoginWithoutPwd: %s', user['username'])
			self.set_secure_cookie("u", user['username'])
			user['authen'] = userInfo['authen'];
			self.write(json.dumps(user, cls=MeEncoder))

			# 登录日志
			loginLog = MeObject(self.appName, 'LoginLog')
			loginLog['username'] = user['username']
			if hasattr(self, 'client_ip'):
				loginLog['ip'] = self.client_ip;
			loginLog.save()
		except Exception,e:
			log.err("LoginWithoutPwd Error: %s Error:%s", self.request.body, str(e))
			self.write(str(e))