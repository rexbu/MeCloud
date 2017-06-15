#-*- coding: utf-8 -*- 
'''
 * file :	DevelopUser.py
 * author :	bushaofeng
 * create :	2016-06-15 21:31
 * func : 
 * history:
'''

from MeObject import *
from MeUser import *
from helper.DbHelper import *
from helper.ClassHelper import *
from helper.Util import *
import json
from datetime import *
import copy

class DevelopUser(MeObject):
	def __init__(self, obj=None):
		MeObject.__init__(self, 'develop', 'User', obj)
		# 0:未认证；1:申请认证；2:已认证；3:未通过认证；4：Library授权；-1:拉黑
		if not self.has_key('authen'):
			self['authen'] = 0;

	### 登录
	def login(self, username=None, password=None):
		if username==None:
			username = self['username']
		if password==None:
			password = self['password']
		userHelper = ClassHelper(self.appDb, self.className)
		user = userHelper.find_one({'username': username, 'password': password})
		if user!=None:
			del(user['password'])
			self.copySelf(user)
			return True
		else:
			return False
	### 注册
	def signUp(self):
		if self['username']==None or self['password']==None:
			raise ERR_NOPARA
		if len(self['username'])<=6 or len(self['password'])<=6:
			raise ERR_PARA

		helper = ClassHelper(self.appDb, self.className)
		print self;
		if(helper.find_one({'username': self['username']})):
			raise ERR_USER_TAKEN
		print 'xxxxxxxxxxxxxxyyyyyyyyy'
		return self.save()