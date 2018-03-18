# coding=utf-8
import json
import traceback

import tornado.websocket
from copy import deepcopy

from mecloud.helper.RedisHelper import RedisDb
from mecloud.model.MeError import ERR_SUCCESS, ERR_COOKIE

from execute import InputtingExecute
from util import RedisUtil
from util.LoggerUtil import logger
from ws import MessageType, Pool


class WsHandler(tornado.websocket.WebSocketHandler):
    userid = None
    cookie = None
    last_ping_on = None
    # user_dict = {}
    dict_body = None

    def data_received(self, chunk):
        logger.debug('data received')
        pass

    def check_origin(self, origin):
        logger.debug('check_origin')
        return True

    def open(self):
        logger.debug('connect open')
        try:
            logger.debug('cookies blow:')
            # logger.debug(self.cookies)
            # logger.debug(self.locale)
            logger.debug(self)
            # for test start
            logger.debug('X-Cookie: %s', self.request.headers['X-Cookie'])
            cookie = self.request.headers['X-Cookie'].split('"')[1]
            logger.debug('cookie: %s', cookie)
            if cookie:
                self.cookie = cookie
                print RedisDb.client
                self.userid = RedisUtil.getUid(cookie)
            logger.debug(self.userid)
            # for test end
            # for test start
            # WsHandler.add_to_dict(self, self)
            # logger.debug('user dict length is below:')
            # logger.debug(Pool.user_dict.__len__())
            # for test end
            if cookie and self.userid:
                WsHandler.add_to_dict(str(self.userid), self)
            else:
                logger.warn('error cookie,非法链接')
                self.go_to_close('error cookie,非法链接')
        except Exception as e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.close()

    def on_message(self, message):
        try:
            logger.debug('on message: %s', message)
            obj = json.loads(message)
            if type(obj) != dict:
                self.go_to_close('wrong message, not json')
            else:
                if dict(obj).has_key('t'):
                    self.dict_body = obj
                    t = obj['t']
                    if t == MessageType.TYPE_MSG_INPUTTING:
                        InputtingExecute.execute(self)
                    elif t == MessageType.TYPE_COMMENT_INPUTTING:
                        InputtingExecute.execute(self)
                    elif t == MessageType.TYPE_MSG_INPUTTING_STOP:
                        InputtingExecute.execute(self)
                    elif t == MessageType.TYPE_COMMENT_INPUTTING_STOP:
                        InputtingExecute.execute(self)
                    elif t == MessageType.TYPE_FRIEND:
                        pass
                    else:
                        self.go_to_close('cannot find a right type of message')
                else:
                    self.go_to_close('wrong message, don not have t')
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.go_to_close('wrong message')

    def on_close(self):
        logger.debug('connect closed')
        if self.userid:
            WsHandler.remove_from_dict(str(self.userid))
        if self.cookie:
            WsHandler.remove_from_dict(str(self.cookie))
        # for test start
        # WsHandler.remove_from_dict(self)
        # for test end
        logger.debug('user dict length is: %d', Pool.user_dict.__len__())

    def go_to_close(self, info=None):
        if info is None:
            info = 'ws_server error'
        # result = deepcopy(ERR_COOKIE.message)
        result = {}
        result['t'] = 'wserror'
        result['code'] = 2000
        result['errCode'] = 2000
        result['errmsg'] = 'cookie error'
        self.write_message(json.dumps(result))
        self.close()

    @staticmethod
    def add_to_dict(userid, handler):
        Pool.user_dict[userid] = handler

    @staticmethod
    def remove_from_dict(userid):
        try:
            Pool.user_dict.pop(userid, False)
        except Exception, e:
            logger.error('userid not in user dict')

# check the message
# def message_is_right(msg):
#     # noinspection PyBroadException
#     try:
#         obj = json.loads(msg)
#         if type(obj) != dict:
#             logger.warn('wrong message')
#             return
#         else:
#             t = obj.get('t', '')
#             if '' != t:
#                 logger.debug('find t, and t is:' + t)
#                 return t
#             else:
#                 logger.warn('t is null, wrong message')
#                 return
#     except Exception as e:
#         logger.error(e)
