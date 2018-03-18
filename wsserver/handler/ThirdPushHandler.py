# coding=utf8
import traceback

import tornado.web
from mecloud.api.BaseHandler import BaseHandler, ERR_PARA, MeObject, ClassHelper, json, MeQuery
from mecloud.helper.RedisHelper import RedisDb

from getui import PushUtil, MyGtUtil
from util.PushUriConfig import action_uri_dict
from ws import Constants
from wsserver import logger


# action_uri_dict = {'assigned': '/assigned', 'claimed': '/claimed', 'followed': '/followed',
#                    'assignedBought': '/assignedBought','ownedBought': '/ownedBought','message': '/message', 'comment': 'comment'}


class ThirdPushHandler(BaseHandler):
    # 根据action获取push的主标题　副标题　内容
    def getNotifyContent(self, action, toid, otherid, extra):
        nickname = ''
        avatar = None
        if otherid:
            other_user1 = MeQuery('UserInfo').find_one({'user': otherid})
            if other_user1:
                nickname = other_user1['nickName']
                avatar = other_user1['avatar']
                logger.debug('nickname:%s, avatar:%s', nickname, avatar)
            else:
                logger.debug('other_user1 is null')
        if action == 'followed':
            title = '新粉丝'
            content = nickname + '关注了你'
        elif action == 'assigned':
            title = nickname
            content = '给你贡献了1张照片,快来看~'
        elif action == 'claimed':
            title = nickname
            content = '认领了你贡献的照片'
        elif action == 'assignedBought':
            title = '获得' + extra + '个蜂蜜新收益'
            content = nickname + '购买了你贡献的照片'
        elif action == 'ownedBought':
            title = '获得' + extra + '个蜂蜜新收益'
            content = nickname + '购买了你的照片'
        elif action == 'newFeed':
            title = nickname
            content = '认领了新照片，快来看~'
        elif action == 'similarFace':
            title = '来自智能探索的新发现'
            if toid:
                toid1 = MeQuery( 'UserInfo' ).find_one( {'user': toid})
                if toid1:
                    nickname = toid1['nickName']
                    if nickname:
                        content = '发现了' + extra + '张可能和你('+ nickname + ')有关的照片'
                    else:
                        content = '发现了' + extra + '张可能和你有关的照片'
        elif action == 'intrestedFace':
            title = '来自智能探索的新发现'
            if toid:
                toid1 = MeQuery( 'UserInfo' ).find_one( {'user': toid})
                if toid1:
                    nickname = toid1['nickName']
                    if nickname:
                        content = '发现了' + extra + '张你('+ nickname + ')可能感兴趣照片'
                    else:
                        content = '发现了' + extra + '张你可能感兴趣照片'
        else:
            logger.warn('push action error:%s', action)
            return None
        return {'title': title, 'content': content, 'avatar': avatar}

    @tornado.web.authenticated
    def get(self, action=None):
        pass

    # @tornado.web.authenticated
    def post(self, action=None):
        if action == 'push':
            self.push()
        else:
            print "action error: " + action

    # 个推push
    def push(self):
        try:
            userid = self.jsonBody['userid']
            otherid = self.jsonBody['otherid']
            action = self.jsonBody.get('action')
            extra = self.jsonBody.get('extra', '')
            logger.debug('third push userid: %s, otherid: %s, action: %s', userid, otherid, action)
            if userid is None or action is None or otherid is None:
                self.write(ERR_PARA.message)
                return
            uri = action_uri_dict.get(action, None)
            if uri is None:
                self.write(ERR_PARA.message)
                return
            uri = 'honey://' + uri
            logger.debug('uri: %s', uri)
            if action == 'claimed' or action == 'newFeed':
                uri = uri + '/' + otherid
            elif action == 'similarFace' or action == 'intrestedFace':
                uri = uri + '?unreadCount=' + extra
            notify_content = self.getNotifyContent(action, userid, otherid, extra)
            logger.debug('notify_content: %s', notify_content)
            if notify_content is None:
                self.write(ERR_PARA.message)
                return
            # 长链接通知
            message_dict = {'t': 'notify'}
            message_dict['title'] = notify_content['title']
            # message_dict['title2'] = notify_content['title2']
            message_dict['subtitle'] = notify_content['content']
            message_dict['avatar'] = notify_content['avatar']
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
            claim_count = 0
            message_count = 0
            stat1 = ClassHelper('StatCount').find_one({'name': 'toClaim_' + userid})
            # logger.debug('stat1:')
            if stat1:
                claim_count = stat1['count']
                if claim_count < 0:
                    claim_count = 0
            stat2 = ClassHelper('StatCount').find_one({'name': 'unreadMsg_' + userid})
            if stat2:
                message_count = stat2['count']
                if message_count < 0:
                    message_count = 0
            badge = claim_count + message_count
            push_cid = MeObject('PushCid', obj=push_cid_obj)
            result = MyGtUtil.pushMessageToSingle(push_cid['cid'], notify_content['title'].decode("utf-8"),
                                                  notify_content['content'].decode("utf-8"), uri, badge)
            logger.debug('result:%s', result)
            # result = PushUtil.pushMessageToSingle()
            self.write(result)
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)
