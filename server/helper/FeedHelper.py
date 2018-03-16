# -*- coding: utf-8 -*-
import json

import requests
from mecloud.api.BaseHandler import BaseConfig
from mecloud.helper.ClassHelper import ClassHelper
from mecloud.helper.RedisHelper import RedisDb
from mecloud.lib import log


def addFeedRecord(userId, feedType, actionId, extraInfo=None):
    """
    给用户添加feed记录
    :param userId: 发出动作的用户
    :param feedType: 行为,包括认领,关注.分享,购买
    :param actionId: 关联的事件id,目前只有认领,actionId为faceid
    :param extraInfo:
    :return:
    """
    faceHelper = ClassHelper('Face')
    face = faceHelper.get(actionId)
    if not face:
        return
    assign = face.get('assign')
    if assign and assign.get('status') == 1:
        assigner = assign.get('assigner')
    else:
        assigner = None
    fansHelper = ClassHelper('Followee')
    feedHelper = ClassHelper('InBox')
    fans = fansHelper.find({'followee': userId, 'effect': {'$gt': 0}})
    feedUserIds = []
    for fan in fans:
        feedInfo = {'user': fan['user'], 'type': feedType, 'actionId': actionId, 'read': False}
        if extraInfo:
            feedInfo['extraInfo'] = extraInfo
        feedHelper.create(feedInfo)
        feedUserIds.append(fan['user'])
    rc = RedisDb.get_connection()
    for user in feedUserIds:
        count = RedisDb.incrby('user_unread_feed_count_%s' % user, 1)
        message_json = {'to_id': user, 'count': count, 't': 'feed', 'uri': 'honey://newFeed/$'+userId}
        publish_result = rc.publish('push_channel', json.dumps(message_json, ensure_ascii=False))
        log.debug('public_result: %s', publish_result)
        # 发推送
        if not assigner == user:
            message = {'userid': user, 'action': 'newFeed', 'otherid': userId}
            requests.post(BaseConfig.pushUrl, data=json.dumps(message), headers={'X-MeCloud-Debug': '1'})

