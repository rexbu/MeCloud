# coding=utf8
import json
import threading
import traceback

import tornado.web
from mecloud.api.BaseHandler import BaseHandler, ERR_PARA, ERR_SUCCESS, MeObject, MeQuery, datetime, date, ObjectId, \
    ClassHelper
from mecloud.helper.RedisHelper import RedisDb

from getui import MyGtUtil
from util import RedisUtil, PushEmoji, SessionUtil
from ws import Constants
from wsserver import logger


# 评论
class CommentHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self, action=None):
        logger.debug('get action:%s', action)
        # logger.debug('json.body:%s', self.jsonBody)
        if action == 'unreadList':
            self.unreadList()
        elif action == 'unreadListByFromId':
            self.unreadListByFromId()
        elif action == 'listByFromId':
            self.listByFromId()
        elif action == 'goodsInfo':
            self.goodsInfo()
        else:
            print "action error: " + action

    @tornado.web.authenticated
    def post(self, action=None):
        logger.debug('post action:%s', action)
        logger.debug('json.body:%s', self.jsonBody)
        if action == 'send':
            self.send()
        elif action == 'read':
            self.read()
        else:
            print "action error: " + action

    # 发消息(评论)
    def send(self):
        try:
            logger.debug(self.jsonBody)
            obj = self.jsonBody
            logger.debug('to_id:' + obj.get('to_id'))
            logger.debug('c: %s', obj.get('c'))
            logger.debug('c_type: %s', obj.get('c_type'))
            logger.debug('m_id: %s', obj.get('m_id'))
            media_id = obj.get('m_id')
            from_id = self.user['_id']
            logger.debug('from_id: %s', from_id)
            user_query = MeQuery("UserInfo")
            user_info = user_query.find_one({'user': from_id})
            message_dict = {'from_id': from_id, 'c': obj.get('c'), 'to_id': obj.get('to_id'),
                            'c_type': obj.get('c_type'), 'msg_type': 2, 'from_avatar': user_info['avatar'], 'from_name'
                            : user_info['nickName'], 'status': 0, 'm_id': media_id}
            message = MeObject('Message', obj=message_dict)
            message['session'] = SessionUtil.create(from_id, obj.get('to_id'))
            message.save()
            # 格式化时间为long型
            message_dict['create_at'] = long(message['createAt'].strftime('%s')) * 1000
            message_dict['t'] = 'comment'
            message_dict['id'] = message['_id']
            message_json = json.dumps(message_dict, ensure_ascii=False)
            logger.debug(message_json)
            print type(message['createAt'])
            rc = RedisDb.get_connection()
            rc.publish(Constants.REDIS_CHANNEL_FOR_PUSH, message_json)
            # 发push
            push_cid_obj = ClassHelper('PushCid').find_one({'userid': obj.get('to_id')})
            logger.debug('push_cid_obj: %s', push_cid_obj)
            if push_cid_obj and (push_cid_obj['logout'] is False):
                # push_cid = MeObject('PushCid', obj=push_cid_obj)
                title = user_info['nickName']
                content = obj.get('c')
                content = PushEmoji.getPushContent(content)
                data = 'honey://comment/' + from_id + '?m_id=' + media_id
                print title.encode('utf-8')
                print content.encode('utf-8')
                claim_count = 0
                message_count = 0
                stat1 = ClassHelper('StatCount').find_one({'name': 'toClaim_' + obj.get('to_id')})
                if stat1:
                    claim_count = stat1['count']
                    if claim_count < 0:
                        claim_count = 0
                stat2 = ClassHelper('StatCount').find_one({'name': 'unreadMsg_' + obj.get('to_id')})
                if stat2:
                    message_count = stat2['count']
                    if message_count < 0:
                        message_count = 0
                badge = claim_count + message_count
                t = threading.Thread(target=MyGtUtil.pushMessageToSingle,
                                     args=(
                                         push_cid_obj['cid'], title.encode('utf-8'), content.encode('utf-8'), data,
                                         badge,))
                t.setDaemon(True)
                t.start()
            # logger.debug(ERR_SUCCESS.message)
            r = {}
            r['id'] = message['_id']
            r['code'] = 0
            r['errCode'] = 0
            r['create_at'] = message_dict['create_at']
            self.write(r)
            # 计数 unreadMsg +1
            logger.debug('update to_id:unreadMsg_%s unreadMsg　count ', obj.get('to_id'))
            ClassHelper('StatCount').updateOne({'name': 'unreadMsg_' + obj.get('to_id')}, {"$inc": {'count': 1}},
                                               upsert=True)
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)

    # 每个人最新的未读评论列表 (session列表)
    def unreadList(self):
        try:
            logger.debug('self.userid: %s:', self.user['_id'])
            all = int(self.get_argument('all', 0))
            logger.debug('all: %s', all)
            userid = self.user['_id']
            logger.debug('userid:%s', userid)
            # userid = '5a0188ccca714319e603c9e8'
            # userid = '5a0d4d88ca71432378862c53'
            if all == 1:
                # match = {"$match": {"to_id": self.user['_id'],
                #                     "msg_type": 2}}
                # {"$or": [{"to_id": to_id, "from_id": from_id}, {"to_id": from_id, "from_id": to_id}]
                match = {"$match": {"$or": [{"to_id": userid}, {"from_id": userid}],
                                    "msg_type": 2}}
            else:
                match = {"$match": {"to_id": userid,
                                    "status": 0, "msg_type": 2}}
            # "59ca0b46ca714306705996dc"
            logger.debug('match:%s', match)
            message_query = MeQuery("Message")
            unread_list = message_query.aggregate([match,
                                                   {"$group": {"_id": {"from_id": "$session", "m_id": "$m_id"},
                                                               # "count": {"$sum": 1},
                                                               "c": {"$last": "$c"},
                                                               "id": {"$last": "$_id"},
                                                               "create_at": {
                                                                   "$last": "$createAt"},
                                                               "c_type": {
                                                                   "$last": "$c_type"},
                                                               "status": {
                                                                   "$last": "$status"},
                                                               "from_name": {
                                                                   "$last": "$from_name"},
                                                               "msg_type": {
                                                                   "$last": "$msg_type"},
                                                               "from_id": {
                                                                   "$last": "$from_id"},
                                                               "to_id": {
                                                                   "$last": "$t"
                                                                            "o_id"},
                                                               "from_avatar": {
                                                                   "$last": "$from_avatar"},
                                                               "m_id": {
                                                                   "$last": "$m_id"}
                                                               }},
                                                   {"$sort": {"create_at": -1}}])  # 时间倒序
            count_match = {"$match": {"to_id": userid, "status": 0, "msg_type": 2}}
            count_list = message_query.aggregate([count_match,
                                                  {"$group": {"_id": {"from_id": "$session", "m_id": "$m_id"},
                                                              "count": {"$sum": 1},
                                                              "from_id": {
                                                                  "$last": "$from_id"},
                                                              "m_id": {
                                                                  "$last": "$m_id"}
                                                              }}])
            logger.debug('unread_list:%s', unread_list)
            logger.debug('count_list:%s', count_list)
            # 处理count_list
            count_dict = {}
            if count_list:
                for count_data in count_list:
                    otherid = count_data['from_id']
                    mid = count_data['m_id']
                    count_dict[otherid + '_' + mid] = count_data['count']
            logger.debug('count_dict:%s', count_dict)
            # 整理数据
            # other_userinfo = None
            if unread_list:
                for unread_msg in unread_list:
                    del unread_msg['_id']
                    # print type(unread_msg['id'])
                    # print str(unread_msg['id'])
                    # 转换objectId to string
                    unread_msg['id'] = str(unread_msg['id'])
                    # print type(unread_msg['create_at'])
                    unread_msg['create_at'] = long(unread_msg['create_at'].strftime('%s')) * 1000
                    if unread_msg['from_id'] == userid:
                        otherid = unread_msg['to_id']
                        # if not other_userinfo:
                        other_userinfo = ClassHelper('UserInfo').find_one({'user': unread_msg['to_id']})
                        logger.debug('other_userinfo:%s', other_userinfo)
                        unread_msg['other_name'] = other_userinfo['nickName']
                        unread_msg['other_avatar'] = other_userinfo.get('avatar', None)
                    else:
                        otherid = unread_msg['from_id']
                        unread_msg['other_name'] = unread_msg['from_name']
                        unread_msg['other_avatar'] = unread_msg['from_avatar']
                    mid = unread_msg['m_id']
                    unread_msg['count'] = count_dict.get(otherid + '_' + mid, 0)

            logger.debug('unread_list after make data :%s', unread_list)
            result = {}
            result['errCode'] = 0
            result['unread_list'] = unread_list
            r = json.dumps(result, ensure_ascii=False)
            self.write(str(r))  # , cls=CJsonEncoder #格式化时间
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)

    # 一个人的未读评论列表
    def unreadListByFromId(self):
        try:
            # test msg_id
            # ObjectId("59dc380051159a123fe8a230")
            # msg_id = self.get_argument('id')
            # logger.debug("id: %s", msg_id)
            print self.user['_id']
            # to_id = "59ca0b46ca714306705996dc"
            to_id = self.user['_id']
            from_id = self.get_argument('from_id')
            m_id = self.get_argument('m_id')
            logger.debug('from_id: %s, m_id: %s', from_id, m_id)
            # if msg_id:
            #     query = {"to_id": to_id, "status": 0,
            #              "_id": {"$lt": ObjectId(msg_id)}}
            # else:
            #     query = {"to_id": to_id, "status": 0}
            query = {"to_id": to_id, "status": 0, "from_id": from_id, "msg_type": 2, "m_id": m_id}
            print query
            helper = ClassHelper('Message')
            cursor = helper.find(query)
            unread_list = []
            if cursor:
                for data in cursor:
                    logger.debug('data:%s', data)
                    unread_list.append(data)
            logger.debug(unread_list)
            # print unread_list.__len__()
            if unread_list:
                for unread_msg in unread_list:
                    unread_msg['id'] = unread_msg['_id']
                    unread_msg['create_at'] = long(unread_msg['createAt'].strftime('%s')) * 1000
                    del unread_msg['_id']
                    if unread_msg.get('acl', None) is not None:
                        del unread_msg['acl']
                    del unread_msg['createAt']
                    del unread_msg['updateAt']
            result = {}
            result['errCode'] = 0
            result['unread_list'] = unread_list
            r = json.dumps(result, ensure_ascii=False)
            self.write(str(r))  # , cls=CJsonEncoder #格式化时间
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)

    # 标记消息已读 message_id为客户端收到的最大的message_id
    def read(self):
        obj = self.jsonBody
        logger.debug('comment read json body:%s', obj)
        to_id = self.user['_id']
        # to_id = '5a018adeca714319e603ca09'
        from_id = obj.get('from_id')
        m_id = obj.get('m_id')
        message_id = obj.get('message_id', None)
        message_ids = obj.get('message_ids', None)
        helper = ClassHelper('Message')
        if message_id:
            r = helper.db.update_many('Message',
                                      {'_id': {'$lte': ObjectId(message_id)}, 'status': 0, 'from_id': from_id,
                                       'msg_type': 2, 'to_id': to_id,
                                       'm_id': m_id},
                                      {'$set': {"status": 1}})
            logger.debug('read comment r:%s', r)
            self.write(ERR_SUCCESS.message)
            if r:
                logger.debug('nModified:%d', r['nModified'])
                if r['nModified'] > 0:
                    logger.debug('reduce name :unreadMsg_%s unreadMsg count', self.user['_id'])
                    ClassHelper('StatCount').updateOne({'name': 'unreadMsg_' + self.user['_id']},
                                                       {"$inc": {'count': -r['nModified']}},
                                                       upsert=False)
        if message_ids:
            ids = []
            for i in str(message_ids).split(','):
                ids.append(i)
            logger.debug('ids:%s', ids)
            r = helper.db.update_many('Message',
                                      {'_id': {'$in': ids}, 'status': 0, 'from_id': from_id,
                                       'msg_type': 2, 'to_id': to_id,
                                       'm_id': m_id},
                                      {'$set': {"status": 1}})
            logger.debug('read comment r:%s', r)
            self.write(ERR_SUCCESS.message)
            if r:
                logger.debug('nModified:%d', r['nModified'])
                if r['nModified'] > 0:
                    logger.debug('reduce name :unreadMsg_%s unreadMsg count', self.user['_id'])
                    ClassHelper('StatCount').updateOne({'name': 'unreadMsg_' + self.user['_id']},
                                                       {"$inc": {'count': -r['nModified']}},
                                                       upsert=False)

    # 格式化时间方法
    class CJsonEncoder(json.JSONEncoder):
        def default(self, obj):
            if isinstance(obj, datetime):
                return obj.strftime('%Y-%m-%d %H:%M:%S')
            elif isinstance(obj, date):
                return obj.strftime('%Y-%m-%d')
            else:
                return json.JSONEncoder.default(self, obj)
            pass

    # 当客户端不带message_id时,返回所有未读,如果一条未读都没有,则按分页返回已读
    def listByFromId(self):
        try:
            # test msg_id
            # ObjectId("59dc380051159a123fe8a230")
            message_id = self.get_argument('message_id', default=None)
            count = int(self.get_argument('count', default=100))
            logger.debug("count: %s", count)
            if count > 100:
                count = 100
            logger.debug("message_id: %s, count: %s", message_id, count)
            # print self.user['_id']
            to_id = self.user['_id']
            # to_id = '5a018adeca714319e603ca09'
            # to_id = '5a018adeca714319e603ca09'
            # to_id = '5a0188ccca714319e603c9e8'
            from_id = self.get_argument('from_id')
            m_id = self.get_argument('m_id')
            logger.debug('from_id: %s, m_id: %s', from_id, m_id)
            helper = ClassHelper('Message')
            unread_list = []
            if not message_id:
                query = {'from_id': from_id, 'to_id': to_id, 'm_id': m_id, 'msg_type': 2, 'status': 0}
                logger.debug('unread query:%s', query)
                cursor = helper.find(query, limit=count, sort={"_id": -1})
                logger.debug('cursor when not message_id:%s', cursor)
                if cursor:
                    for data in cursor:
                        logger.debug('data:%s', data)
                        unread_list.append(data)
                logger.debug('real unread_list:%s', unread_list)
            if not unread_list:
                if message_id:
                    query = {"$or": [{"to_id": to_id, "from_id": from_id}, {"to_id": from_id, "from_id": to_id}],
                             "msg_type": 2, "m_id": m_id, "_id": {"$lt": ObjectId(message_id)}}
                else:
                    query = {"$or": [{"to_id": to_id, "from_id": from_id}, {"to_id": from_id, "from_id": to_id}],
                             "msg_type": 2, "m_id": m_id}
                logger.debug('query: %s', query)
                cursor = helper.find(query, limit=count, sort={"_id": -1})
                if cursor:
                    for data in cursor:
                        logger.debug('data:%s', data)
                        unread_list.append(data)
                logger.debug('unread_list:%s', unread_list)
            if unread_list:
                unread_list.reverse()
                for unread_msg in unread_list:
                    unread_msg['id'] = unread_msg['_id']
                    unread_msg['create_at'] = long(unread_msg['createAt'].strftime('%s')) * 1000
                    del unread_msg['_id']
                    if unread_msg.get('acl', None) is not None:
                        del unread_msg['acl']
                    del unread_msg['createAt']
                    del unread_msg['updateAt']
                    if unread_msg['from_id'] == to_id:
                        unread_msg['status'] = 1
            result = {}
            result['errCode'] = 0
            result['unread_list'] = unread_list
            r = json.dumps(result, ensure_ascii=False)
            self.write(str(r))  # , cls=CJsonEncoder #格式化时间
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)

    def goodsInfo(self):
        try:
            userid = self.user['_id']
            # userid = '5a08fbfbca714330cfd35ddc'
            message_id = self.get_argument('message_id', None)
            message = ClassHelper('Message').get(message_id)
            if not message:
                self.write(ERR_PARA.message)
                return
            logger.debug('message:%s', message)
            file_id = message['m_id']
            media = ClassHelper('Media').find_one({'file': file_id})
            logger.debug('media:%s', media)
            face = None
            if media:
                face = self.getFace(media, message['from_id'], message['to_id'])
                logger.debug('face1 %s:', face)
                if not face:
                    face['goodsId'] = None
                    face['mosaic'] = 2
                    face['price'] = 0
                    logger.debug('未找到face')
                else:
                    face['goodsId'] = None
                    face['mosaic'] = 10
                    face['price'] = 0
                    if face.get('assign', None):
                        owner_id = face['assign']['user']
                        good = ClassHelper('Goods').find_one({'goods': face['_id']})
                        if good:
                            face['user'] = face['assign']['user']
                            face['goodsId'] = good['_id']
                            price = good['price'] if good else 0
                            face['price'] = price

                            if media.get('uploader', None) and media['uploader'] == userid:
                                # 照片贡献者
                                face['mosaic'] = 0
                            elif owner_id == userid:
                                # 照片认领者
                                face['mosaic'] = 5
                            if face['mosaic'] != 0:
                                charge_record = ClassHelper('ChargeFlow').find_one(
                                    {'user': userid, 'goods': good['_id'], 'status': 1}) if good else None
                                logger.debug('charge_record%s', charge_record)
                                if charge_record:
                                    face['mosaic'] = 1
                        else:
                            logger.warn('goods is null')
                            # self.write(ERR_PARA.message)
                            # return
            userInfo = {}
            if media:
                logger.debug('uploader:%s', media.get('uploader', None))
                user_info_obj = ClassHelper('UserInfo').find_one({'user': media.get('uploader', None)})
                logger.debug('user_info_obj:%s', user_info_obj)
                if user_info_obj:
                    userInfo['_id'] = user_info_obj['_id']
                    userInfo['avatar'] = user_info_obj['avatar']
                    userInfo['nickName'] = user_info_obj['nickName']
            logger.debug('userInfo:%s', userInfo)
            if media and face:
                r = {}
                r['errCode'] = 0
                r['width'] = media.get('width', 0)
                r['height'] = media.get('height', 0)
                r['media'] = media['_id']
                r['file'] = media['file']
                r['goodsId'] = face['goodsId']
                r['mosaic'] = face['mosaic']
                r['price'] = face['price']
                r['_id'] = face['_id']
                r['user'] = face.get('user', None)
                r['userInfo'] = userInfo
                if face.get('rect', None) is not None:
                    r['rect'] = face.get('rect')
                r['message_id'] = message_id
                self.write(r)
            else:
                r = {}
                r['errCode'] = 0
                r['width'] = 0
                r['height'] = 0
                r['media'] = None
                r['file'] = None
                r['goodsId'] = None
                r['mosaic'] = 0
                r['price'] = 0
                r['_id'] = None
                r['user'] = None
                r['rect'] = None
                r['userInfo'] = None
                r['message_id'] = message_id
                self.write(r)
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)

    def getFace(self, media, userid1, userid2):
        faces = media.get('faces', [])
        logger.debug('faces:%s', faces)
        if not faces:
            return {}
        face_helper = ClassHelper('Face')
        face = {}
        for faceId in faces:
            face = face_helper.get(faceId)
            logger.debug('face:%s', face)
            if not face:
                face = {}
                continue
            assign = face.get('assign', None)
            if not assign:
                continue
            if (assign.get('user', None) == userid1 or assign.get('user', None) == userid2) and assign.get(
                    'status') == 1:
                return face
        return face
