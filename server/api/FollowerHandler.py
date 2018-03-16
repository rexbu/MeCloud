#!/user/bin/env python
# encoding: utf-8
'''
@author: Dong Jun
@file:   FollowerHandler.py
@time:   2017/9/21  21:12
'''

import json
import tornado.web
from copy import deepcopy
from bson import ObjectId
from mecloud.api.BaseHandler import BaseHandler, BaseConfig
from mecloud.helper.ClassHelper import ClassHelper
from mecloud.helper.Util import MeEncoder
from mecloud.lib import log
from mecloud.model.MeError import *
from mecloud.api.CountHandler import getIFCCount
from mecloud.helper.FollowHelper import *



class FollowerHandler(BaseHandler):
    # def __init__(self):
    #     super(FollowerHandler, self).__init__()

    @tornado.web.authenticated
    def get(self, followType):
        # classHelper = ClassHelper("Followee")
        # query = []
        # matchDict = {}
        # matchEffect = {}
        # backDict = {}
        # infoDict = {}
        # noUserDict = {}
        # userId = None
        # try:
        #     if self.request.arguments.has_key('userId'):
        #         userId = self.get_argument('userId')
        # except Exception:
        #     self.write(ERR_NOPARA.message)
        #     return
        # if followType == "Followee":  # 关注   # == & is
        #     matchDict = {"$match": {"user": userId}}
        #     matchEffect = {"$match": {"effect": 1}}
        #     backDict = {"$lookup": {
        #         "from": "Followee",
        #         "localField": "followee",
        #         "foreignField": "user",
        #         "as": "status"
        #     }}
        #     infoDict = {"$lookup": {
        #         "from": "UserInfo",
        #         "localField": "followee",
        #         "foreignField": "user",
        #         "as": "UserInfo"
        #     }}
        #     noUserDict = {"$lookup": {
        #         "from": "BackupUser",
        #         "localField": "backupFollowee",
        #         "foreignField": "_sid",
        #         "as": "backupInfo"
        #     }}
        # elif followType == "Follower":  # 粉丝
        #     matchDict = {"$match": {"followee": userId}}
        #     matchEffect = {"$match": {"effect": 1}}
        #     backDict = {"$lookup": {
        #         "from": "Followee",
        #         "localField": "user",
        #         "foreignField": "followee",
        #         "as": "status"
        #     }}
        #     infoDict = {"$lookup": {
        #         "from": "UserInfo",
        #         "localField": "user",
        #         "foreignField": "user",
        #         "as": "UserInfo"
        #     }}
        # else:
        #     self.write(ERR_PARA.message)
        #     return
        #
        # sortDict = {"$sort": {"updateAt": -1}}  # sort is fixed to -1
        # limitDict = {}
        # startidDict = {}
        # skipDict = {}
        # try:
        #     if self.request.arguments.has_key('startId'):
        #         startId = self.get_argument('startId')
        #         startidDict = {"$match": {"_sid": {"$lt": startId}}}
        #
        #     if self.request.arguments.has_key('skip'):
        #         skipNum = int(self.get_argument('skip'))
        #     else:
        #         skipNum = 0
        #     skipDict = {"$skip": skipNum}
        #
        #     limit = 20
        #     if followType == "Followee":  # == & is
        #         limit = 100
        #         if self.request.arguments.has_key('pageSize'):
        #             limit = int(self.get_argument('pageSize'))
        #             limit = min(limit, 100)
        #     elif followType == "Follower":
        #         limit = 20
        #         if self.request.arguments.has_key('pageSize'):
        #             limit = int(self.get_argument('pageSize'))
        #             limit = min(limit, 20)
        #     limitDict = {"$limit": limit}
        #
        # except Exception:
        #     self.write(ERR_INVALID.message)
        #     return
        #
        # # displayDict = {"$project": {"UserInfo": 1, "status": 1, "followee": 1,"_sid": 1, "isuser": 1,
        # #                             "backupInfo": 1, "effect": 1, "user": 1, "updateAt": 1}}
        #
        # displayDict = {'$project':
        #     {
        #         'status': 1, 'backupInfo.nickName': 1, 'backupInfo._id': 1, 'backupInfo.avatar': 1,
        #         'UserInfo.nickName': 1, 'UserInfo.user': 1, 'UserInfo._id': 1, 'UserInfo.avatar': 1, 'user': 1,
        #         'followee': 1, 'backupFollowee': 1, "updateAt": 1
        #     }
        # }
        #
        # try:
        #     query.append(matchDict)
        #     query.append(matchEffect)
        #     query.append(sortDict)
        #     if startidDict != {}:
        #         query.append(startidDict)
        #     query.append(skipDict)
        #     query.append(limitDict)
        #     query.append(backDict)
        #     query.append(infoDict)
        #     if noUserDict:
        #         query.append(noUserDict)
        #     # query.append(lunchDict)
        #     query.append(displayDict)
        #
        #     followList = classHelper.aggregate(query)
        #
        #     if followList is not None:
        #         for num in range(len(followList)):
        #             if followType == "Followee":
        #                 followList[num]["fansInfo"] = {}
        #                 for item in followList[num]['status']:
        #                     fUserId = item.get('followee') or item.get('backupFollowee')
        #                     if fUserId == userId:
        #                         followList[num]["fansInfo"]['_sid'] = item["_sid"]
        #                         followList[num]["fansInfo"]['effect'] = item["effect"]
        #             elif followType == "Follower":
        #                 followList[num]["fansCount"] = len(followList[num]["status"])
        #                 followList[num]["followInfo"] = {}
        #                 for item in followList[num]['status']:
        #                     if item['user'] == userId:
        #                         followList[num]["followInfo"]['effect'] = item['effect']
        #                         followList[num]["followInfo"]['_sid'] = item['_sid']
        #
        #             followList[num].pop('status', None)
        #             try:
        #                 if followList[num]["UserInfo"]:
        #                     followList[num]["UserInfo"] = followList[num]["UserInfo"][0]
        #                     followList[num]["UserInfo"].pop("acl", None)
        #                     followList[num]["UserInfo"].pop("createAt", None)
        #                     userInput = followList[num]["UserInfo"]["user"]
        #                     followList[num].update(getIFCCount(userId=userInput))
        #                 else:
        #                     followList[num]["UserInfo"] = {}
        #                 if "backupInfo" in followList[num] and followList[num]["backupInfo"]:
        #                     followList[num]["backupInfo"] = followList[num]["backupInfo"][0]
        #                     followList[num]["backupInfo"].pop("acl", None)
        #                     followList[num]["backupInfo"].pop("createAt", None)
        #                     userInput = followList[num]["backupInfo"]["_id"]
        #                     followList[num].update(getIFCCount(backupUser=userInput))
        #                 else:
        #                     followList[num]["backupInfo"] = {}
        #             except Exception, ex:
        #                 log.err("FollowerHandler Err: %s", ex)
        #     else:
        #         self.write(ERR_INVALID.message)
        #         return
        #     # final data for querying
        #     successInfo = deepcopy(ERR_SUCCESS)
        #     successInfo.message["data"] = followList
        #     self.write(json.dumps(successInfo.message, cls=MeEncoder))
        #
        # except Exception, ex:
        #     data = deepcopy(ERR_INVALID)
        #     data.message['data'] = ex.message
        #     self.write(json.dumps(data.message, cls=MeEncoder))
        #     return

        try:
            if self.request.arguments.has_key('userId'):
                userId = self.get_argument('userId')
        except Exception:
            self.write(ERR_NOPARA.message)
            return

        try:
            lastTime=None
            count=10
            is_user = True
            skipNum = 0
            if self.request.arguments.has_key('lastTime'):
                lastTime = self.get_argument('lastTime')

            if followType == "followee":  # == & is
                if self.request.arguments.has_key( 'count' ):
                    count = int( self.get_argument( 'count' ) )
                    count = min( count, 100 )
            elif followType == "follower":
                if self.request.arguments.has_key( 'count' ):
                    count = int( self.get_argument( 'count' ) )
                    count = min( count, 20 )

            if self.request.arguments.has_key('isUser'):
                if int(self.get_argument('isUser')) == 0:
                    is_user = False

            if self.request.arguments.has_key( 'skip' ):
                skipNum = int( self.get_argument( 'skip' ) )

            if followType == "followee": #关注   # == & is
                followList = FollowHelper.getFollowees(userId, lastTime, count, skipNum)
            elif followType == "follower":#粉丝
                followList = FollowHelper.getFollowers(userId, lastTime, count, is_user, skipNum)
            else:
                self.write(ERR_PATH_PERMISSION.message)
                return

            # final data for querying
            successInfo = deepcopy(ERR_SUCCESS)
            successInfo.message["data"] = followList
            self.write(json.dumps(successInfo.message, cls=MeEncoder))

        except Exception,ex:
            data = deepcopy(ERR_INVALID)
            data.message['data'] = ex.message
            self.write(json.dumps(data.message, cls=MeEncoder))
            return

    @tornado.web.authenticated
    def post(self, action=None, followee=None, isuser=1):
        userId = self.get_current_user()
        if not userId:
            log.err("follow error,user not exist!")
            self.write(ERR_USER_NOTFOUND.message)
            return

        if not followee:
            log.err("request param error")
            self.write(ERR_NOPARA.message)
            return

        is_user = True
        if int(isuser) == 0:
            is_user = False

        # 查找用户是否存在
        if is_user == True:
            userHelper = ClassHelper( "User" )
        else:
            userHelper = ClassHelper( "BackupUser" )
        findUser = userHelper.find_one( {'_sid': followee} )
        if not findUser:
            log.err( "%s error,followee not exist!",action)
            self.write( ERR_USER_NOTFOUND.message )
            return

        blackHelper = ClassHelper( "Blacklist" )
        if action == 'follow':
            blacked = blackHelper.find_one( {'user': userId, 'blacker': followee} )
            isblacked = blackHelper.find_one( {'user': followee, 'blacker': userId} )
            if blacked:
                self.write( ERR_BLACKED_PERMISSION.message )
                return
            if isblacked:
                self.write( ERR_BEBLACKED_PERMISSION.message )
                return

        if is_user:
            fieldname = "followee"
        else:
            fieldname = "backupFollowee"
        followHelper = ClassHelper("Followee")

        try:
            if action == 'follow':  # 关注
                # 判断是否已经关注过
                followed = followHelper.find_one( {'user': userId, fieldname: followee, 'effect': {'$gte': 0}} )
                if followed and followed['effect']>=1:
                    self.write( ERR_SUCCESS.message )
                    return
                FollowHelper.follow(userId, followee, is_user)
                fr = followHelper.find_one( {'user': userId, fieldname: followee, 'effect': {'$gte': 1}},
                                            {'acl': 0, 'createAt': 0, '_sid': 0} )
                fr['relationStatus']=fr['effect']
                del fr['effect']
                del fr['_id']

                if fr.has_key('backupFollowee'):
                    del fr['backupFollowee']
                    fr['isUser'] = 0
                else:
                    del fr['followee']
                    fr['isUser'] = 1

                successInfo = deepcopy( ERR_SUCCESS )
                successInfo.message["data"] = fr
                self.write( json.dumps( successInfo.message, cls=MeEncoder ) )
                # 需要从用户推荐中删除
                recommendHelper = ClassHelper('UserRecommend')
                recommend = recommendHelper.find_one({'user': userId, 'recommender': followee})
                if recommend:
                    recommendHelper.delete(recommend['_id'])
            elif action == 'unfollow':  # 取消关注
                # 之前未关注过或者已经取消关注，直接返回
                unfollowed = followHelper.find_one( {'user': userId, fieldname: followee, 'effect': {'$gte': 1}} )
                if not unfollowed:
                    self.write( ERR_SUCCESS.message )
                    return
                FollowHelper.unfollow(userId, followee, is_user)
                fr = followHelper.find_one( {'user': userId, fieldname: followee, 'effect': {'$gte': 0}},
                                            {'acl': 0, 'createAt': 0, '_sid': 0} )
                fr['relationStatus'] = fr['effect']
                del fr['effect']
                del fr['_id']

                if fr.has_key( 'backupFollowee' ):
                    del fr['backupFollowee']
                    fr['isUser'] = 0
                else:
                    del fr['followee']
                    fr['isUser'] = 1

                successInfo = deepcopy( ERR_SUCCESS )
                successInfo.message["data"] = fr
                self.write( json.dumps( successInfo.message, cls=MeEncoder ) )
            else:
                print "action error: " + action
                self.write( ERR_PATH_PERMISSION.message )
                return
        except Exception, e:
            log.err("FollowerHandler-->action:%s in post() error, %s", action, e)
            self.write(ERR_DB_OPERATION.message)
            return
        if action == 'follow':
            if is_user and not followed:
                try:
                    # 计数 newFans_ +1
                    ClassHelper('StatCount').updateOne({'name': 'newFans_' + followee},
                                                       {"$inc": {'count': 1}},
                                                       upsert=True)
                except Exception, e:
                    print e
                    self.write( ERR_DB_OPERATION.message )
                    return

                self.pushMessage( followee, userId )


    @tornado.web.asynchronous
    def pushMessage(self, followee, user):
        def callback(response):
            log.info( 'Push:%s', response.body)
            self.finish()

        pushUrl = BaseConfig.pushUrl
        pushData = {
            'userid': followee,
            'action': 'followed',
            'otherid': user
        }
        client = tornado.httpclient.AsyncHTTPClient()
        client.fetch(pushUrl, callback=callback, method="POST", body=json.dumps(pushData), headers={'X-MeCloud-Debug': '1'})
