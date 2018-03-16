#-*- coding: utf-8 -*- 
'''
 * file :	MeObject.py
 * author :	bushaofeng
 * create :	2017-01-12 17:37
 * func : 
 * history:
'''

from mecloud.helper.ClassHelper import *
from MeObject import *

class MeQuery(ClassHelper):
    def __init__(self, className):
        ClassHelper.__init__(self, className)

    ### 通过id获取对象
    def get(self, oid):
        o = ClassHelper.get(self, oid)
        if o!=None:
            return MeObject(self.className, o)
        return None
        
	### 查询第一个对象	
    def find_one(self, query, keys=None):
        o = ClassHelper.find_one(self, query, keys)
        if o!=None:
            return MeObject(self.className, o)
        return None
	
	### 查询
    def find(self, query, keys=None, sort=None, limit=0):
        ClassHelper.find(self, query, keys, sort, limit)
        return self
    
    """
    查询迭代器
    """
    def __iter__(self):
        return self
    def next(self):
        return MeObject(self.className, ClassHelper.next(self))
    __next__ = next
