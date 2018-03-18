#!/user/bin/env python
# encoding: utf-8
'''
@author: Li Ji
@file:   FollowerHandler.py
@time:   2017/11/23  11:50
'''

import json
from copy import deepcopy
from mecloud.api.BaseHandler import BaseHandler
from mecloud.helper.Util import MeEncoder
from mecloud.model.MeError import *
from mecloud.api.CountHandler import *
from mecloud.helper.FollowHelper import *
from mecloud.helper.ClassHelper import *


class FollowerHandler(BaseHandler):
    def get(self, followType):
        try:
            if self.request.arguments.has_key('userId'):
                userId = self.get_argument('userId')
        except Exception:
            self.write(ERR_NOPARA.message)
            return

        try:
            lastTime = None
            count = 10
            is_user = True
            if self.request.arguments.has_key('lastTime'):
                lastTime = self.get_argument('lastTime')

            if followType == "followee":  # == & is
                count = 100
                if self.request.arguments.has_key('count'):
                    count = int(self.get_argument('count'))
                    count = min(count, 100)
            elif followType == "follower":
                count = 20
                if self.request.arguments.has_key('count'):
                    count = int(self.get_argument('count'))
                    count = min(count, 20)

            if self.request.arguments.has_key('isUser'):
                if int(self.get_argument('isUser')) == 0:
                    is_user = False

            if followType == "followee":  # 关注   # == & is
                followList = FollowHelper.getFollowees(userId, lastTime, count)
            elif followType == "follower":  # 粉丝
                followList = FollowHelper.getFollowers(userId, lastTime, count, is_user)
            else:
                self.write(ERR_PATH_PERMISSION.message)
                return

            if followList is not None:
                for num in range(len(followList)):
                    try:
                        if followList[num]["UserInfo"]:
                            followList[num]['isUser'] = 1
                            userInput = followList[num]["UserInfo"]["user"]
                            cia = get_follow_ount(userInput, 1)
                            if cia:
                                followList[num]['countInfo'] = cia
                            if followType == "follower":
                                blacked = ClassHelper('Blacklist').find_one( {'user': userId, 'blacker': userInput})
                                if blacked:
                                    followList[num]['blackStatus'] = 1
                        else:
                            followList[num]["UserInfo"] = {}
                            followList[num]['isUser'] = 0
                        if "backupInfo" in followList[num] and followList[num]["backupInfo"]:
                            userInput = followList[num]["backupInfo"]["_id"]
                            cia = get_follow_ount(userInput, 0)
                            if cia:
                                followList[num]['countInfo'] = cia
                            if followType == "follower":
                                blacked = ClassHelper( 'Blacklist' ).find_one( {'user': userId, 'blacker': userInput} )
                                if blacked:
                                    followList[num]['blackStatus'] = 1
                        else:
                            followList[num]["backupInfo"] = {}
                    except Exception, ex:
                        log.err("FollowerHandler Err: %s", ex)

                for num in range(len(followList)):
                    del followList[num]["user"]
                    del followList[num]["_id"]
                    if followList[num].has_key( "followee" ):
                        del followList[num]["followee"]
                    if followList[num].has_key( "backupFollowee" ):
                        del followList[num]["backupFollowee"]

                    if followList[num].has_key( "UserInfo" ):
                        if followList[num]["UserInfo"]:
                            if followList[num]["UserInfo"].has_key( "nickName" ):
                                followList[num]["nickName"] = followList[num]["UserInfo"]["nickName"]
                            if followList[num]["UserInfo"].has_key( "avatar" ):
                                followList[num]["avatar"] = followList[num]["UserInfo"]["avatar"]
                            if followList[num]["UserInfo"].has_key( "address" ):
                                followList[num]["address"] = followList[num]["UserInfo"]["address"]
                            followList[num]["user"] = followList[num]["UserInfo"]["user"]
                        del followList[num]["UserInfo"]

                    if followList[num].has_key( "backupInfo" ):
                        if followList[num]["backupInfo"]:
                            if followList[num]["backupInfo"].has_key( "nickName" ):
                                followList[num]["nickName"] = followList[num]["backupInfo"]["nickName"]
                            if followList[num]["backupInfo"].has_key( "editAvatar" ):
                                followList[num]["avatar"] = followList[num]["backupInfo"]["editAvatar"]
                            elif followList[num]["backupInfo"].has_key( "avatar" ):
                                followList[num]["avatar"] = followList[num]["backupInfo"]["avatar"]
                            followList[num]["user"] = followList[num]["backupInfo"]["_id"]
                            if followList[num]["backupInfo"].has_key( "editRect" ):
                                followList[num]["rect"] = followList[num]["backupInfo"]["editRect"]
                            elif followList[num]["backupInfo"].has_key( "rect" ):
                                followList[num]["rect"] = followList[num]["backupInfo"]["rect"]
                            if followList[num].has_key( "rect" ):
                                rect = followList[num]["rect"]
                                rect = [int(i) for i in rect]
                                followList[num]["rect"] = rect
                        del followList[num]["backupInfo"]

                    if followList[num].has_key('countInfo') and followList[num]['countInfo']:
                        count_key = followList[num]['countInfo'].keys()
                        for key in count_key:
                            followList[num][key] = followList[num]['countInfo'][key]
                        del followList[num]["countInfo"]

                    if followType == "follower" and int( followList[num]["effect"] ) == 1:
                        followList[num]["relationStatus"] = 3
                    else:
                        followList[num]["relationStatus"] = followList[num]["effect"]
                    del followList[num]["effect"]
            else:
                self.write(ERR_INVALID.message)
                return
            # final data for querying
            successInfo = deepcopy(ERR_SUCCESS)
            successInfo.message["data"] = followList
            self.write(json.dumps(successInfo.message, cls=MeEncoder))
            # 计数 newFans_ 清空
            if followType == "follower":
                ClassHelper('StatCount').updateOne({'name': 'newFans_' + self.user['_id']},
                                                   {"$set": {'count': 0}},
                                                   upsert=True)
        except Exception, ex:
            data = deepcopy(ERR_INVALID)
            data.message['data'] = ex.message
            self.write(json.dumps(data.message, cls=MeEncoder))
            return