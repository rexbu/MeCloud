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


class IncomeHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self, action=None):
        try:
            if action == 'flow':
                self.flow()
            else:
                print 'action error ', action
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)

    @tornado.web.authenticated
    def post(self, action=None):
        pass

    def flow(self):
        id = self.get_argument('id', None)
        size = int(self.get_argument('size', 10))
        userid = self.user['_id']
        # userid = '59dc3decca7143413c03a62f'

        if id:
            query = {'user': userid, "_id": {"$lt": ObjectId(id)}}
        else:
            query = {'user': userid}

        cursor = ClassHelper('IncomeFlow').find(query, limit=size)
        data_list = []
        if cursor:
            for data in cursor:
                data['id'] = data['_id']
                del data['_id']
                # logger.debug('new time:%s', datetime.strftime(datetime.now(), '%Y-%m-%d %H:%M:%S.%f%Z'))
                # new_date = datetime.strptime(data['createAt'], '%Y-%m-%d %H:%M:%S.%f%Z')
                create_at = data['createAt'].strftime('%Y-%m-%d %H:%M:%S')
                logger.debug('create_at:%s', create_at)
                data['create_at'] = create_at
                data['mosaic'] = 1
                del data['_sid']
                del data['updateAt']
                del data['createAt']
                del data['user']
                data_list.append(data)
        logger.debug('data_list size:%d', data_list.__len__())
        result = {}
        result['code'] = 0
        result['errCode'] = 0
        result['flows'] = data_list
        self.write(result)
        # 新收益计数清空
        ClassHelper('StatCount').updateOne({'name': 'newIncome_' + userid},
                                           {"$set": {'count': 0}},
                                           upsert=True)
