#!/user/bin/env python
# encoding: utf-8
'''
@author: Li Ji
@file:   DeviceMigrateHandler.py
@time:   2017/12/5  11:50
'''

import json
from copy import deepcopy
from mecloud.api.BaseHandler import BaseHandler, BaseConfig
from mecloud.helper.Util import MeEncoder
from mecloud.model.MeError import *
from mecloud.api.CountHandler import *
from mecloud.helper.FollowHelper import *
from mecloud.helper.ClassHelper import *


class DeviceMigrateHandler(BaseHandler):
    def get(self):
        userHelper = ClassHelper( "User" )
        userMigrateHelper = ClassHelper( "UserMigrate" )

        userId = self.get_current_user()
        if not userId:
            log.err( "DeviceMigrateHandler error,user not exist in cookie!" )
            self.write( ERR_USER_NOTFOUND.message )
            return

        findUser = userHelper.find_one( {'_sid': userId})
        if not findUser:
            log.err( "DeviceMigrateHandler error,user not exist in User!" )
            self.write( ERR_USER_NOTFOUND.message )
            return

        if not findUser.has_key('device'):
            log.err( "DeviceMigrateHandler error,user not exist without device!" )
            self.write( ERR_USER_NOTFOUND.message )
            return

        try:
            migrate = userMigrateHelper.find_one({'user': userId, 'device': findUser['device'], 'isused':0})
            if not migrate:
                query = {'user': userId, 'device': findUser['device'], 'isused':0}
                userMigrateHelper.updateOne( query, {'$set': {'createAt':datetime.now(),'acl': {userId: {"write": 'true'}, "*": {"read": 'true'}}}}, upsert=True)
            else:
                import time
                createAt = migrate['createAt']
                curAt = datetime.now()
                gap= time.mktime( curAt.timetuple() ) - time.mktime( createAt.timetuple())
                if gap > 600:
                    userMigrateHelper.update(migrate['_id'], {'$set': {'isused': 1}})
                    query = {'user': userId, 'device': findUser['device'], 'isused':0}
                    userMigrateHelper.updateOne( query, {'$set': {'createAt':datetime.now(),'acl': {userId: {"write": 'true'}, "*": {"read": 'true'}}}}, upsert=True )
        except Exception, e:
            log.err( "DeviceMigrateHandler error, %s", e )
            self.write( ERR_DB_OPERATION.message )
            return

        migrate = userMigrateHelper.find_one({'user': userId, 'device': findUser['device'], 'isused': 0})
        url =BaseConfig.wsserver
        successInfo = deepcopy( ERR_SUCCESS )
        migrate['url'] = migrate['_id']
        successInfo.message["data"] = migrate
        self.write( json.dumps( successInfo.message, cls=MeEncoder ) )
