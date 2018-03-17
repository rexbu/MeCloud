# -*- coding: utf-8 -*-
'''
 * file :	MeRole.py
 * author :	bushaofeng
 * create :	2016-06-09 17:37
 * func : 
 * history:
'''

from MeObject import *
from MeUser import *
from MeRelation import *
from mecloud.helper.DbHelper import *
from mecloud.helper.ClassHelper import *
from mecloud.helper.Util import *
import json
from datetime import *
import copy


class MeRole(MeObject):
    ### 初始化函数，name和_id都是Role的唯一id
    def __init__(self, name):
        MeObject.__init__(self, 'MeRole')
        self['name'] = name
        self['users'] = {'_type': 'relation', '_class': self.className}

    def addUser(self, user):
        if self.objectId == None:
            self.save()
        userRelation = MeRelation(self.className)
        userRelation.relate(self, user)
        userRelation.save()

    ### 检查user是否输出本角色
    def contains(self, user):
        if not (type(user) is MeUser):
            raise TypeError('user must a MeUser')
        if user.objectId == None:
            raise TypeError('user must has saved')
        if self.objectId == None and (not self.fetch()):
            return False
        relate = MeRelation(self.className)
        relation = relate.relation(self, user)
        return relation != None

    ### 检查某个角色是否本角色子角色
    def child(self, role):
        pass

    ### 从数据库中获取对象
    def fetch(self):
        roleClass = ClassHelper(self.className)
        role = roleClass.find_one({'name': self['name']})
        if role == None:
            return False
        self.dirty.clear()
        self.objectId = obj['_id']
        for k in obj:
            dict.__setitem__(self, k, obj[k])
        return True
