# coding=utf8
import traceback

import tornado.web
from mecloud.api.BaseHandler import BaseHandler, ERR_PARA, MeObject, ClassHelper, ERR_SUCCESS, deepcopy, json, \
    ERR_DEVICE_ALREADY_ACTIVE
from mecloud.helper.RedisHelper import RedisDb

from getui import MyGtUtil
# from mecloud.model.MeError import ERR_DEVICE_ALREADY_ACTIVE
from util import InviteCodeUtil
from ws import Constants
from wsserver import logger


class InviteCodeHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self, action=None):
        try:
            if action == 'createInviteCode':
                self.createInviteCode()
            elif action == 'createUserInviteCode':
                self.createUserInviteCode()
            elif action == 'owned':
                self.owned()
            else:
                print 'action error ', action
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)

    def post(self, action=None):
        try:
            if action == 'check':
                self.check()
            elif action == 'activate':
                self.activate()
            else:
                print 'action error ', action
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)

    def owned(self):
        # '5a0a7f88ca714378820c022f'
        invite_code = ClassHelper('InviteCode').find_one({'from': self.user['_id'], 'status': 0})
        result = deepcopy(ERR_SUCCESS.message)
        code = None
        if invite_code:
            code = invite_code['code']
        result['code'] = code
        self.write(result)

    def createInviteCode(self):
        count = int(self.get_argument('count', 1))
        if count > 100:
            count = 100
        i = 0
        while (True):
            try:
                invite_code = MeObject('InviteCode')
                invite_code['code'] = InviteCodeUtil.create_code()
                invite_code['status'] = 0
                invite_code.save()
                i = i + 1
                logger.debug('create invite code finish %d', i)
                if i >= count:
                    break
            except Exception, e:
                logger.error(e)
        logger.debug('all finish')

    def createUserInviteCode(self):
        cursor = ClassHelper('User').find({})
        if cursor:
            i = 0
            for u in cursor:
                code = ClassHelper('InviteCode').find_one({'from': u['_id'], 'status': 0})
                n = 0
                if not code:
                    while (True):
                        try:
                            invite_code = MeObject('InviteCode')
                            invite_code['code'] = InviteCodeUtil.create_code()
                            invite_code['status'] = 0
                            invite_code['from'] = u['_id']
                            invite_code.save()
                            n = n + 1
                            logger.debug('create invite code success')
                            self.notify_new_code(u['_id'])
                            # logger.debug('create invite code finish %d', i)
                            if n >= 1:
                                break
                        except Exception, e:
                            logger.error(e)
                else:
                    logger.debug('user already has a code')
                i = i + 1
                logger.debug('finish %d', i)
            logger.debug('all finish')

    def notify_new_code(self, userid):
        # 长链接通知
        title = '恭喜获得了一枚新的黑蜜邀请码'
        content = '快分享给朋友一起来玩吧'
        uri = 'honey://newInviteCode'
        message_dict = {'t': 'notify'}
        message_dict['title'] = title
        message_dict['subtitle'] = content
        message_dict['avatar'] = None
        message_dict['to_id'] = userid
        message_dict['uri'] = uri
        message_json = json.dumps(message_dict, ensure_ascii=False)
        logger.debug('publish_message:%s', message_json)
        rc = RedisDb.get_connection()
        publish_result = rc.publish(Constants.REDIS_CHANNEL_FOR_PUSH, message_json)
        logger.debug('publish_result: %s', publish_result)
        push_cid_obj = ClassHelper('PushCid').find_one({'userid': userid})
        logger.debug('push_cid_obj: %s', push_cid_obj)
        if (push_cid_obj is None) or push_cid_obj['logout'] is True:
            # 没有找到对应的push_cid
            self.write(ERR_PARA.message)
            return
        push_cid = MeObject('PushCid', obj=push_cid_obj)
        result = MyGtUtil.pushMessageToSingle(push_cid['cid'], title.decode("utf-8"),
                                              content.decode("utf-8"), data=uri)
        logger.debug('push result:%s', result)

    def check(self):
        # code = self.jsonBody.get('code')
        device = self.jsonBody.get('device')
        logger.debug('active check, device:%s', device)
        active_device = ClassHelper('ActiveDevice').find_one({'device': device})
        if active_device:
            logger.debug('device:%s active true', device)
            r = {'errCode': 0, 'active': True}
        else:
            logger.debug('device:%s active false', device)
            r = {'errCode': 0, 'active': False}
        self.write(r)

    def activate(self):
        logger.debug('invite code activate start ')
        code = self.jsonBody.get('code')
        device = self.jsonBody.get('device')
        invite_code = ClassHelper('InviteCode').find_one({'code': code, 'status': 0})
        logger.debug('jsonBody:%s', self.jsonBody)
        if not invite_code:
            logger.debug('invite code error')
            result = deepcopy(ERR_PARA.message)
            result['errMsg'] = '邀请码错误'
            result['msg'] = 'invite code error'
            self.write(result)
            return
        else:
            active_device = MeObject('ActiveDevice')
            active_device['device'] = device
            active_device['code'] = code
            try:
                active_device.save()
                invite_code_obj = MeObject('InviteCode', obj=invite_code)
                invite_code_obj['status'] = 1
                # invite_code_obj['to'] = self.user['_id']
                invite_code_obj.save()
                logger.debug('success')
                self.write(ERR_SUCCESS.message)
                return
            except Exception, e:
                logger.debug('device already active')
                logger.error(e)
                msg = traceback.format_exc()
                logger.error(msg)
                result = deepcopy(ERR_DEVICE_ALREADY_ACTIVE.message)
                result['errMsg'] = '设备已激活,不能重复激活'
                result['msg'] = 'device already active'
                self.write(result)
                return
