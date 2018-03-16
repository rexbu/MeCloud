#!/user/bin/env python
# encoding: utf-8
'''
@author: Dong Jun
@file:   BlacklistHandler.py
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


class BlacklistHandler(BaseHandler):
    @tornado.web.authenticated
    def post(self, action=None, blackee=None, isuser=1):
        userId = self.get_current_user()
        if not userId:
            log.err("black error,user not exist!")
            self.write(ERR_USER_NOTFOUND.message)
            return

        if not blackee:
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
        findUser = userHelper.find_one( {'_sid': blackee} )
        if not findUser:
            log.err( "%s error,blackee not exist!",action)
            self.write( ERR_USER_NOTFOUND.message )
            return

        blackHelper = ClassHelper("Blacklist")
        followHelper = ClassHelper( "Followee" )

        try:
            if action == 'block':  # 拉黑
                # 判断是否已经拉黑过
                blacked = blackHelper.find_one( {'user': userId, 'blacker': blackee})
                if blacked:
                    self.write(ERR_SUCCESS.message)
                    return
                blackHelper.create({'user': userId, 'blacker': blackee})

                if is_user:
                    fieldname = "followee"
                else:
                    fieldname = "backupFollowee"
                unfollowed = followHelper.find_one( {'user': userId, fieldname: blackee, 'effect': {'$gte': 1}} )
                if unfollowed:
                    FollowHelper.unfollow(userId, blackee, is_user)
                br = blackHelper.find_one( {'user': userId, 'blacker': blackee})
                successInfo = deepcopy( ERR_SUCCESS )
                successInfo.message["data"] = br
                self.write( json.dumps( successInfo.message, cls=MeEncoder ) )
            elif action == 'unblock':  # 取消拉黑
                unblacked = blackHelper.find_one( {'user': userId, 'blacker': blackee} )
                if not unblacked:
                    self.write( ERR_SUCCESS.message )
                    return
                blackHelper.delete(unblacked['_id'])
                self.write( ERR_SUCCESS.message )
            else:
                print "action error: " + action
                self.write( ERR_PATH_PERMISSION.message)
                return
        except Exception, e:
            log.err("BlacklistHandler-->action:%s in post() error, %s", action, e)
            self.write(ERR_DB_OPERATION.message)