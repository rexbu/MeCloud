# coding=utf8
import json

import copy
import tornado.web
from mecloud.api.BaseHandler import BaseHandler
from mecloud.helper.ClassHelper import ClassHelper
from mecloud.helper.Util import MeEncoder, checkKeysAndValue, checkKeys
from mecloud.lib import log
from mecloud.model.MeError import *
from mecloud.model.MeObject import MeObject
from mecloud.model.MeQuery import MeQuery
from mecloud.model.MeUser import MeUser
from mecloud.helper.CountHelper import *


class ShareRecordHandler(BaseHandler):
    # @tornado.web.authenticated
    def get(self, action=None):
        if action == "sharerecord":#根据分享记录返回人或face的信息
            if not self.request.arguments.has_key('compareId'):
                self.write(ERR_PATH_PERMISSION.message)
                return
            compareId = self.get_argument("compareId")
            shareRecordHelper = ClassHelper("ShareRecords")
            srItem = shareRecordHelper.get(compareId)
            if srItem:
                srItem = self.filter_field(srItem)
                result = {}
                if srItem['shareType'] < 2:
                    item = self.getUser(srItem['shareTargetId'],srItem['shareType'])
                    if item:
                        result['nickname'] = item['nickName']
                        result['avatar'] = item['avatar']
                        if "user" in item:
                            result['userId'] = item['user']
                        else:
                            result['userId'] = item['_id']  
                else:
                    result = self.getFace(srItem['shareTargetId'])
                result.update(srItem)
                self.write(json.dumps(result, cls=MeEncoder))
            else:
                self.write(ERR_PATH_PERMISSION.message)

        else:
            self.write(ERR_PATH_PERMISSION.message)

    def getUser(self, userId, isUser=-1):
        if isUser == 1:
            userHelper = ClassHelper("UserInfo")
            obj = userHelper.find_one({'user': userId}) or {}
        else:
            obj = ClassHelper("BackupUser").get(userId) or {}
        if not obj:
            userHelper = ClassHelper("UserInfo")
            obj = userHelper.find_one({'user':userId}) or {}
        return self.filter_field(obj)


    def getFace(self, faceId):
        faceHelper = ClassHelper("Face")
        obj = faceHelper.find_one(query={"_id":faceId},keys={"rect":1,"possible.score":1,"file":1}) or {}
        item = {}
        item['faceId'] = faceId
        item['rect'] = obj['rect']
        item['file'] = obj['file']
        item['score'] = obj['possible'][0]['score']
        # user = {}
        # if obj:
        #     if "uploader" in obj:
        #         user = self.getUser(obj['uploader'],1)
        #     elif "backupUser" in obj:
        #         user = self.getUser(obj['backupUser'], 0)
        #     else:
        #         pass
        #     user = self.filter_field(user)
        # obj = self.filter_field(obj)
        return item


