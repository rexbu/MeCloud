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


class ShareHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self, action=None):
        if action == "user":
            classHelper = ClassHelper('Media')
            items = classHelper.aggregate(
                                [
                                    {
                                        # '$match': {'uploader': "59dc3decca7143413c03a62f", 'publish': False}
                                        '$match': {'uploader': self.user['_id'], 'publish': False}
                                    },
                                    {
                                        '$project': {'faces': 1}
                                    },
                                    {
                                        '$unwind': "$faces"
                                    },
                                    {
                                        "$lookup": {
                                            "from": "Face",
                                            "localField": "faces",
                                            "foreignField": "_sid",
                                            "as": "faceInfo"
                                        }
                                    },
                                    {
                                        '$unwind': "$faceInfo"
                                    },
                                    {
                                        "$project": {"faces": 1, "possible": "$faceInfo.possible", 'media': "$faceInfo.media"}
                                    },
                                    {
                                        "$unwind": "$possible"
                                    },
                                    {
                                        '$group': {
                                            "_id": {'userId': "$possible.user", "backupUserId": "$possible.backupUser"},
                                            "media": {'$first': "$media"},
                                            "count": {'$sum': 1}
                                        }
                                    },
                                    {"$sort": {"count": -1}},
                                    {
                                        '$lookup': {
                                            "from": "UserInfo",
                                            "localField": "_id.userId",
                                            "foreignField": "user",
                                            "as": "userInfo"
                                        }
                                    },
                                    {
                                        '$lookup': {
                                            "from": "BackupUser",
                                            "localField": "_id.backupUserId",
                                            "foreignField": "_sid",
                                            "as": "backupUserInfo"
                                        }
                                    }
                                ]
            )
            userInfo = []
            for obj in items:
                try:
                    if obj['userInfo']:
                        userInfo.append({'nickName':obj['userInfo'][0].get('nickName',None),'userId':obj["_id"]['userId'],'count':obj['count']})
                    else:
                        userInfo.append({'nickName': obj['backupUserInfo'][0].get('nickName', None), 'backupUserId': obj["_id"]['backupUserId'],'count':obj['count']})
                except Exception,e:
                    log.err("MyHandler-->get user(backup) info err for userId:%s", obj['_id'])
            data = copy.deepcopy(ERR_SUCCESS)
            data.message['data'] = userInfo
            self.write(json.dumps(data.message, cls=MeEncoder))
        elif action == "share":#获取分享文案
            if self.request.arguments.has_key('platform') and self.request.arguments.has_key('type'):
                #where={'platform':"","type":""}
                query = {
                    "platform": self.get_argument('platform'),
                    "type": self.get_argument('type')
                }
                # query['type'] = "app"
                classHelper = ClassHelper("ShareCopywrite")
                objs = classHelper.find_one(query)

                if objs['type'] == "app":#分享app
                    objs = self.filter_field(objs)
                    if "file" in objs:
                        objs['imageUrl'] = self.getFile(objs['file'])
                else:#分享user或者backupUser或者分享图片
                    if objs['type'] == "image":  # 分享图片
                        if self.request.arguments.has_key('userId'):
                            item = self.getUser(self.get_argument("userId"))
                        else:
                            item = {}
                    else:#分享user或者backupUser
                        if not self.request.arguments.has_key('userId'):
                            self.write(ERR_PATH_PERMISSION.message)
                            return
                        isUser = 1
                        if objs['type'] == "backupUser":
                            isUser = 0
                        userId = self.get_argument("userId")
                        item = self.getCountUser(userId, isUser)
                        objs['url'] = objs['url'] + "?userId=" + userId + "&isUser=" + str(isUser)
                    if item:
                        try:
                            item['nickName'] = item['nickname']
                        except:
                            pass
                        try:
                            objs['title'] = objs['title'].format(**item)
                        except:
                            objs.pop("title", None)
                        try:
                            objs['subTitle'] = objs['subTitle'].format(**item)
                        except:
                            objs.pop("subTitle", None)
                        try:
                            objs['file'] = objs['file'].format(**item)
                        except:
                            objs.pop("file", None)
                        # if objs.get('file', None) == "{avatar}":
                        #     objs['imageUrl'] = self.getFile(item['avatar'])
                    else:
                        objs.pop("title",None)
                        objs.pop("file",None)
                        objs.pop("subTitle",None)
                objs = self.filterField(objs)
                self.write(json.dumps(objs, cls=MeEncoder))
            else:
                self.write(ERR_PATH_PERMISSION.message)
        elif action == "qrcode":#返回二维码需要的url
            if not self.request.arguments.has_key('shareTargetId') or not self.request.arguments.has_key('shareType'):
                self.write(ERR_PATH_PERMISSION.message)
                return
            shareType = int(self.get_argument("shareType"))
            shareTargetId = self.get_argument("shareTargetId")
            # 记录图片分享
            try:
                shareRecordHelper = ClassHelper("ShareRecords")
                shareInfo = shareRecordHelper.create({
                    "from": self.user['_id'],
                    "shareTargetId": shareTargetId,
                    "shareType": shareType,
                    "compareFaceId": self.get_argument("compareFaceId")
                })
                shareInfo['url'] = "http://heyhoney.blinnnk.com/" + "?compareId=" + shareInfo['_id']
                self.write(json.dumps(shareInfo, cls=MeEncoder))
            except Exception,ex:
                # print ex
                self.write(ERR_PATH_PERMISSION.message)
        else:
            self.write(ERR_PATH_PERMISSION.message)

    def getFile(self,fileId):
        fileHelper = ClassHelper('File')
        obj = fileHelper.get(fileId)
        fileUrl = "http://"+obj['bucket']+".oss-cn-beijing.aliyuncs.com/"+obj['name']
        return fileUrl
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
    
    def getCountUser(self, userId, isUser=1):
        item = profile(userId, isUser)
        obj = copy.deepcopy(item)
        item['assignersCount'] = item.get('assigners',0)
        item['followersCount'] = item.get('followers',0)
        item['imageCount'] = item.get('medias',0)
        faceHelper = ClassHelper('Face')
        toClaimCount = faceHelper.query_count({'assign.user': userId, 'assign.status': 0}) or 0
        item['toClaimCount'] = toClaimCount
        if item['assignersCount'] and item['followersCount'] and item['imageCount'] and item['toClaimCount']:
            return item
        return obj

    def filterField(self, obj):
        obj = self.filter_field(obj)
        # obj.pop("file",None)
        obj.pop("platform", None)
        return obj



