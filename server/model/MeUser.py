#-*- coding: utf-8 -*- 
'''
 * file :	MeUser.py
 * author :	bushaofeng
 * create :	2016-06-09 17:37
 * func : 
 * history:
'''

from MeObject import *
from helper.DbHelper import *
from helper.ClassHelper import *
from helper.Util import *
import json
from datetime import *
import copy

class MeUser(MeObject):
	def __init__(self, obj=None):
		MeObject.__init__(self, 'User', obj)

	def login(self, username, password):
		userHelper = ClassHelper(self.className)
		user = userHelper.find_one({'username': username, 'password': password})
		if user!=None:
			self.copySelf(user)
			del(user['username'])
			del(user['password'])
			return True
		else:
			return False

	### 注册
	def signup(self, username=None, password=None):
		self.save()

	### 修改密码
	def modPwd(self, username, password, newPwd):
		pass

	def loginWithoutPwd(self):
		appUserHelper = ClassHelper('develop', "AppUser")
		appUser = appUserHelper.find_one({'username': self['username']})
		obj = copy.deepcopy(self.dirty)
		if appUser==None:
			# 涉及到不同的应用，所以AppUser不存储bundleId和package
			if obj.has_key('bundleId'):
				del(obj['bundleId'])
			elif obj.has_key('package'):
				del(obj['package'])
			appUser = MeObject('develop', 'AppUser', obj)
			appUser.save()
		# 如果用户发生系统更新
		else:
			appUser = MeObject('develop', 'AppUser', appUser)
			if obj['system']!=appUser['system']:
				appUser['system'] = obj['system']
				appUser.save()

		userHelper = ClassHelper(self.appDb, 'User')
		user = userHelper.find_one({'username': obj['username']})
		if user==None or (not user.has_key('appUserId')):
			self['appUserId'] = appUser.objectId
			self.save()
		else:
			self.copySelf(user)
			self.dirty.clear()
	