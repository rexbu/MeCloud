# coding=utf8
import traceback

import tornado.web
from mecloud.api.BaseHandler import BaseHandler, ERR_INVALID, ClassHelper, MeObject, ERR_SUCCESS
from mecloud.helper.RedisHelper import RedisDb

from wsserver import logger


class LogoutHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self, action=None):
        pass

    @tornado.web.authenticated
    def post(self, action=None):
        try:
            logger.debug('Cookie: %s', self.request.headers['Cookie'])
            cookie = self.request.headers['Cookie'].split('"')[1]
            RedisDb.delete(cookie)
            self.clear_cookie('u')
            push_cid_obj = ClassHelper('PushCid').find_one({'userid': self.user['_id']})
            if push_cid_obj:
                push_cid = MeObject('PushCid', obj=push_cid_obj)
                push_cid['logout'] = True
                push_cid.save()
            self.write(ERR_SUCCESS.message)
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_INVALID.message)
