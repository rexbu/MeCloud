#-*- coding: utf-8 -*- 
'''
 * file :	MeACL.py
 * author :	bushaofeng
 * create :	2016-06-18 22:24
 * func : 
 * history:
'''

from MeRole import *
from MeUser import *
from DevelopUser import *
from MeRelation import *
from helper.DbHelper import *
from helper.ClassHelper import *
from helper.Util import *
import json
from datetime import *
import copy

class MeACL(dict):
	def __init__(self, acl=None):
		if acl==None:
			self['*'] = {'read': True, 'write': True}
		else:
			if not (isinstance(acl, dict)):
				raise TypeError('acl must a dict')
			for k in acl:
				self[k] = acl[k]
	### 设置公共读权限
	def setPublicReadAccess(self, auth=True):
		if not self.has_key("*"):
			self['*'] = {}
		self['*']['read'] = auth
	### 设置公共写权限
	def setPublicWriteAccess(self, auth=True):
		if not self.has_key("*"):
			self['*'] = {}
		self['*']['write'] = auth
	### 设置角色读权限
	def setRoleReadAccess(self, role, auth=True):
		if not isinstance(role, MeRole):
			raise TypeError('role must a MeRole')
		if not self.has_key('role:'+role['name']):
			self['role:'+role['name']] = {}
		self['role:'+role['name']]['read'] = auth
	### 设置角色写权限
	def setRoleWriteAccess(self, role, auth=True):
		if not isinstance(role, MeRole):
			raise TypeError('role must a MeRole')
		if not self.has_key('role:'+role['name']):
			self['role:'+role['name']] = {}
		self['role:'+role['name']]['write'] = auth
	### 设置用户读权限
	def setUserReadAccess(self, user, auth=True):
		if not (isinstance(user, DevelopUser) or isinstance(user, MeUser)):
			raise TypeError('user must a MeUser')
		if user.objectId == None:
			raise TypeError('user must has saved')
		if not self.has_key(user.objectId):
			self[user.objectId] = {}
		self[user.objectId]['read'] = auth
	### 设置用户写权限
	def setUserWriteAccess(self, user, auth=True):
		if not (isinstance(user, DevelopUser) or isinstance(user, MeUser)):
			raise TypeError('user must a MeUser')
		if user.objectId == None:
			raise TypeError('user must has saved')
		if not self.has_key(user.objectId):
			self[user.objectId] = {}
		self[user.objectId]['write'] = auth

	### 获取用户是否有读权限
	def readAccess(self, appName, user):
		if self.has_key('*') and self['*'].has_key('read') and self['*']['read']:
			return True
		# 如果user不为*，那么必须有为MeUser
		if not (type(user) is MeUser):
			return False

		# 如果明确指定了某个用户的权限，则不再检查Role
		if self.has_key(user.objectId) and self[user.objectId].has_key('read'):
			return self[user.objectId]['read']
		# 检查role
		for k in self:
			if k.startswith('role:'):
				role = MeRole(appName, k[5:])
				# 如果有一个角色有读权限，那么就有读权限
				if role.contains(user) and self[k].has_key('read') and self[k]['read']:
					return True
		return False
					
	### 获取用户是否有写权限
	def writeAccess(self, appName, user):
		if self.has_key('*') and self['*'].has_key('write') and self['*']['write']:
			return True
		# 如果user不为*，那么必须有为MeUser
		if not (type(user) is MeUser):
			return False

		# 如果明确指定了某个用户的权限，则不再检查Role
		if self.has_key(user.objectId) and self[user.objectId].has_key('write'):
			return self[user.objectId]['write']
		# 检查role
		for k in self:
			if k.startswith('role:'):
				role = MeRole(appName, k[5:])
				# 如果有一个角色有写权限，那么就有写权限
				if role.contains(user) and self[k].has_key('write') and self[k]['write']:
					return True
		return False