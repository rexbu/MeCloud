# -*- coding: utf-8 -*-
'''
 * file :	MeObject.py
 * author :	bushaofeng
 * create :	2016-06-09 17:37
 * func : 
 * history:
'''
from bson import ObjectId
import copy


class MeObject(dict):
    ### 初始化
    def __init__(self, className, obj=None, overLoadGet=False):
        self.className = className
        # 标记是否和数据库中一个document对应
        self.objectId = None
        # 存储未和数据库同步的数据
        self.dirty = {}
        # 标记是否重载[]运算符
        self.overLoadSet = True
        # 嵌套情况下是否直接提取_content，false时候取
        self.overLoadGet = overLoadGet
        if obj != None:
            if not isinstance(obj, dict):
                raise TypeError('obj must a dict object')
            # 如果存在_id字段，则拷贝到自己
            elif obj.has_key('_id'):
                self.copySelf(obj, self.overLoadGet)
                # 在拷贝一个Object情况下不在设置acl
                return
            else:
                self.dirty = copy.deepcopy(obj)
        # 权限，有循环导入问题
        # from MeACL import MeACL
        # if obj != None and obj.has_key('acl'):
        #     acl = MeACL(obj['acl'])
        #     self.dirty['acl'] = acl.updateSuperAccess()
        # else:
        #     self.dirty['acl'] = MeACL()

    ### 根据id建一个空的对象
    @staticmethod
    def createWithId(className, oid):
        obj = MeObject(className)
        obj.setOverLoad('_id', oid)
        obj.objectId = oid
        return obj

    ### 重载索引
    def __getitem__(self, key):
        if self.dirty.has_key(key):
            obj = self.dirty[key]
        elif self.has_key(key):
            obj = dict.__getitem__(self, key)
        else:
            return None
        # 如果是Object引用，直接返回MeObject
        if isinstance(obj, dict) and obj.has_key('_type') and obj['_type'] == 'pointer':
            if self.overLoadGet:
                if obj.has_key('_content'):
                    return obj['_content']
                else:
                    mobj = MeObject(obj['_class'])
                    mobj.get(obj['_id'])
                    return mobj
            else:
                if not obj.has_key('_content'):
                    mobj = MeObject(obj['_class'])
                    mobj.get(obj['_id'])
                    obj['_content'] = mobj
        return obj

    ### 赋值
    def __setitem__(self, key, val):
        if not self.overLoadSet:
            return dict.__setitem__(self, key, val)

        # 如果是已经是数据库中的对象，且没有初始化dirty
        if self.objectId != None and not self.dirty.has_key('$set'):
            self.dirty['$set'] = {}

        if isinstance(val, MeObject):
            # 没有保存到数据库则保存
            if val.objectId == None:
                val.save()
            obj = {}
            obj['_type'] = 'pointer'
            obj['_id'] = val.objectId
            obj['_class'] = val.className
            if self.objectId != None:
                self.dirty['$set'][key] = obj
            else:
                self.dirty[key] = obj
        # 不推荐走这个分支!!!
        elif isinstance(val, dict) and val.has_key('_type') and val['_type'] == 'pointer':
            # 如果有_id，表示是已经存储的object
            if val.has_key('_id'):
                obj = {}
                obj['_type'] = 'pointer'
                obj['_id'] = result['_id']
                obj['_class'] = val.className
                if self.objectId != None:
                    self.dirty['$set'][key] = obj
                else:
                    self.dirty[key] = obj
            # 没有_id,新建object，存储
            else:
                mobj = MeObject(obj['_class'], obj['_content'])
                mobj.save()
                self[key] = mobj
        else:
            if self.objectId != None:
                self.dirty['$set'][key] = val
            else:
                self.dirty[key] = val

    ### 直接对dict赋值，不进dirty
    def setOverLoad(self, key, val):
        dict.__setitem__(self, key, val)

    def append(self, key, val):
        if self.objectId != None and not self.dirty.has_key('$push'):
            self.dirty['$push'] = {}

        if type(val) is MeObject:
            # 没有保存到数据库则保存
            if val.objectId == None:
                result = val.save()
            obj = {}
            obj['_type'] = 'pointer'
            obj['_id'] = result['_id']
            obj['_class'] = val.className
            if self.objectId != None:
                self.dirty['$push'][key] = obj
            else:
                if not self.dirty.has_key(key):
                    self.dirty[key] = []
                self.dirty[key].append(obj)
        # 不推荐走这个分支!!!
        elif isinstance(val, dict) and val.has_key('_type') and val['_type'] == 'pointer':
            # 如果有_id，表示是已经存储的object
            if val.has_key('_id'):
                obj = {}
                obj['_type'] = 'pointer'
                obj['_id'] = result['_id']
                obj['_class'] = val.className
                if self.objectId != None:
                    self.dirty['$push'][key] = obj
                else:
                    if not self.dirty.has_key(key):
                        self.dirty[key] = []
                    self.dirty[key].append(obj)
            # 没有_id,新建object，存储
            else:
                mobj = MeObject(obj['_class'], obj['_content'])
                self.append(key, mobj)
        else:
            if self.objectId != None:
                self.dirty['$push'][key] = val
            else:
                if not self.dirty.has_key(key):
                    self.dirty[key] = []
                self.dirty[key].append(obj)

    ### 保存
    def save(self):
        from mecloud.helper.ClassHelper import ClassHelper
        classHelper = ClassHelper(self.className)
        if self.objectId == None:
            if self.dirty.has_key('_sid'):
                self.dirty['_id'] = ObjectId(self.dirty['_sid'])
                del (self.dirty['_sid'])
            obj = classHelper.create(self.dirty)
            if obj==None:
                return False

            self.objectId = obj['_id']
        else:
            obj = classHelper.updateWithId(self.objectId, self.dirty)
            if obj==None:
                return False
                
        for k in obj:
            dict.__setitem__(self, k, obj[k])

        self.dirty.clear()
        return True

    ### 从数据库中读取, child:是否将子对象放到_content字段下，默认放
    def get(self, oid, child=False):
        from mecloud.helper.ClassHelper import ClassHelper
        classHelper = ClassHelper(self.className)
        obj = classHelper.get(oid)
        # if obj == None:
        #     return False
        self.copySelf(obj, child)
        self.dirty.clear()
        return obj

    ### 如果某个成员是MeObject类型，获取
    def fetch(self, k):
        if isinstance(self[k], dict) and self[k].has_key('_type') and self[k]['_type'] == 'pointer':
            db = self.appDb
            if self[k].has_key('_db'):
                db = self[k]['_db']
            if self[k]['_class'] == 'MeFile':
                child = MeFile(db)
            else:
                child = MeObject(db, self[k]['_class'])
            child.get(obj[k]['_id'])
            return child

    ### 删除对象
    @staticmethod
    def delete(oid):
        from mecloud.helper.ClassHelper import ClassHelper
        classHelper = ClassHelper(self.className)
        classHelper.delete(oid)

    def copySelf(self, obj, child=False):
        self.objectId = obj['_id']
        for k in obj:
            # 如果是引用类型，取出引用内容
            if isinstance(obj[k], dict) and obj[k].has_key('_type') and obj[k]['_type'] == 'pointer':
                db = self.appDb
                if obj[k].has_key('_db'):
                    db = obj[k]['_db']
                if obj[k]['_class'] == 'MeFile':
                    from MeFile import MeFile
                    childObj = MeFile(db)
                else:
                    childObj = MeObject(db, obj[k]['_class'], None, self.overLoadGet)
                childObj.get(obj[k]['_id'])
                if child:
                    obj[k] = childObj
                else:
                    obj[k]['_content'] = childObj
            dict.__setitem__(self, k, obj[k])
