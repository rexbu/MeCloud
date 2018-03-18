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


class CountHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self, action=None):
        pass
        if action == 'index':
            self.index()
        else:
            print 'action error ', action

    @tornado.web.authenticated
    def post(self, action=None):
        pass

    def index(self):
        try:
            userid = self.user['_id']
            # userid = '5a1aea98ca714333dda69c5c'
            cursor = ClassHelper('StatCount').find({'name': {
                '$in': ['unreadMsg_' + userid, 'newFans_' + userid, 'toClaim_' + userid, 'newIncome_' + userid]}})
            unread_msg = 0
            fans = 0
            unclaimed = 0
            earnings = 0
            if cursor:
                for data in cursor:
                    if data['name'] == 'unreadMsg_' + userid:
                        unread_msg = data['count']
                    elif data['name'] == 'newFans_' + userid:
                        fans = data['count']
                    elif data['name'] == 'toClaim_' + userid:
                        unclaimed = data['count']
                    elif data['name'] == 'newIncome_' + userid:
                        earnings = data['count']
            result = {}
            result['code'] = 0
            result['errCode'] = 0
            result['fans'] = fans
            # result['unread_msg'] = unread_msg
            result['unread_msg'] = ClassHelper('Message').query_count({'to_id': userid, 'status': 0})
            result['earnings'] = earnings
            result['unclaimed'] = unclaimed
            result['feedCount'] = int(RedisDb.get('user_unread_feed_count_%s' % userid) or 0)
            faceHelper = ClassHelper( 'FaceRecommend' )
            sid = RedisDb.hget( "recommendSLatestOid", userid)
            if sid:
                query={'_sid': {"$lte": sid},'user': userid, 'read': {'$exists': False},'backupUser': {'$exists': False}}
                result['similarCount'] = faceHelper.query_count(query)
            else:
                result['similarCount'] = 0
            recommendHelper = ClassHelper('UserRecommend')
            result['possibleCount'] = recommendHelper.query_count({'user': userid})
            self.write(result)
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)
