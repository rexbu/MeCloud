#-*- coding: utf-8 -*- 

from bson import json_util
from BaseHandler import *
from model.MeObject import *
from model.MeQuery import *
from model.MeError import *
from helper.DbHelper import *
from helper.ClassHelper import *
from helper.Util import *
import json
from datetime import *

class ClassHandler(BaseHandler):
    ### 获取对象 及 批量查询
    @tornado.web.authenticated
    def get(self, className, objectId=None):
        if objectId:
            obj = MeObject(self.appName, className)
            return self.write(obj.get(objectId))
        else:
            classHelper = ClassHelper(self.appDb, className)
            query = {}
            if self.request.arguments.has_key('where'):
                query = eval(self.get_argument('where'))
            if self.request.arguments.has_key('keys'):
                keys = eval(self.get_argument('keys'))
            else:
                keys = None
            objs = classHelper.find(query, keys)
            objects = []
            for obj in objs:
                mo = MeObject(self.appDb, className, obj, False)
                mo.overLoadGet = False
                objects.append(mo)
            self.write(json.dumps(objects, cls=MeEncoder));

    ### 创建对象, 返回ObjectId, createAt, updateAt
    def post(self, className):
        try:
            obj = json.loads(self.request.body)
        except Exception,e:
            log.err("JSON Error:%s , error:%s", self.request.body, str(e))
            self.write(ERR_INVALID.message)
            return
            
        print 'className:'+className;
        meobj = MeObject(className, obj)
        meobj.save()
        self.write(json.dumps(meobj, cls=MeEncoder))

    ### 更新对象
    def put(self, className, objectId):
        if not objectId:
            self.write(ERR_OBJECTID_MIS.message)
            return
        try:
            obj = json.loads(self.request.body)
        except Exception,e:
            log.err("JSON Error:[%d/%s] , error:%s", len(self.request.body), self.request.body, str(e))
            self.write(ERR_INVALID.message)
            return
        classHelper = ClassHelper(self.appDb, className)
        # 只返回了更新时间
        data = classHelper.update(objectId, obj)
        # 默认返回整个对象
        self.write(json.dumps(data, cls=MeEncoder))

    ### 删除对象
    def delete(self, className, objectId):
        if not objectId:
            self.write(MeException(ERR_PARA_OBJECTID, "No ObjectId."))
            return
        classHelper = ClassHelper(self.appDb, className)
        classHelper.delete(objectId)
    	self.write('true')
