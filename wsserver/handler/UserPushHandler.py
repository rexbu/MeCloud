# coding=utf8
import json
import traceback

import tornado.web
from mecloud.api.BaseHandler import BaseHandler, ERR_PARA, ERR_SUCCESS, MeObject, MeQuery, datetime, date, ObjectId, \
    ClassHelper
from mecloud.helper.RedisHelper import RedisDb

from getui import demo2
from util import RedisUtil
from ws import Constants
from wsserver import logger


class UserPushHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self, action=None):
        pass

    @tornado.web.authenticated
    def post(self, action=None):
        logger.debug('action: %s', action)
        if action == 'saveCid':
            self.saveCid()
        else:
            print "action error: " + action

    # 新建或者更新push token
    def saveCid(self):
        try:
            cid = self.jsonBody['cid']
            platform = self.jsonBody['platform']
            logger.debug('cid: %s, platform: %s', cid, platform)
            userid = self.user['_id']
            # userid = '5a016819ca71430a9d0fe108'
            logger.debug('userid: %s', userid)
            push_cid_obj = ClassHelper('PushCid').find_one({'userid': userid})
            print push_cid_obj
            push_cid = MeObject('PushCid', obj=push_cid_obj)
            print type(push_cid)
            push_cid['userid'] = userid
            push_cid['platform'] = platform
            push_cid['cid'] = cid
            push_cid['logout'] = False
            push_cid.save()
            self.write(ERR_SUCCESS.message)
            r = ClassHelper('PushCid').db.update_many('PushCid',
                                                      {'cid': cid, 'userid': {'$ne': userid}, 'logout': False},
                                                      {'$set': {'logout': True}})
            logger.debug('r:%s', r)
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)
