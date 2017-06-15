#-*- coding: utf-8 -*- 
'''
 * file :	MeRelation.py
 * author :	bushaofeng
 * create :	2016-06-13 18:17
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

class MeRelation(MeObject):
	def __init__(self, className):
		MeObject.__init__(self, className+'Relation')

	# 添加o1 o2的关系
	def relate(self, o1, o2):
		if not isinstance(o1, MeObject):
			raise TypeError('object1 must a MeObject')
		if not isinstance(o1, MeObject):
			raise TypeError('object2 must a MeObject')
		self[o1.className] = o1;
		self[o2.className] = o2;
	
	def add(self, o):
		if not self.has_key('objects'):
			self['objects'] = []
		self['objects'] = self.append('objects', o)

	### 返回 o1 o2的关系
	def relation(self, o1, o2):
		relateHelper = ClassHelper(self.className)
		return relateHelper.find({o1.className+'._id': o1.objectId, 
			o2.className+'._id': o2.objectId})