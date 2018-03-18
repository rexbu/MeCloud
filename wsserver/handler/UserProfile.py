# -*- coding: utf-8 -*-
"""
用户主页相关
"""
from copy import deepcopy

from mecloud.helper.ClassHelper import ClassHelper
from mecloud.model.MeError import ERR_SUCCESS, tornado, ERR_PATH_PERMISSION, ERR_INVALID
from mecloud.lib import log
from mecloud.api.BaseHandler import BaseHandler
# from handler.FacerBase import FacerBase, getMosicLevel


class UserProfile(BaseHandler):
    PAGE_PER_COUNT = 20
    USER_FACE_THRESHOLD = 0.4

    @tornado.web.authenticated
    def get(self, object_id, action, isUser=1):
        """
        获取个人主页照片列表
        :param object_id:
        :param action:
        :return:
        """
        if action == "list":
            result = deepcopy(ERR_SUCCESS.message)
            user_id = self.get_current_user()
            updateAt = self.get_argument('updateAt', '')
            face_helper = ClassHelper('Face')
            media_helper = ClassHelper('Media')
            GoodHelper = ClassHelper('Goods')
            userHelper = ClassHelper('User')
            user = userHelper.get(object_id)
            keys = {'media': 1, 'file': 1, 'rect': 1, 'updateAt': 1, 'assign': 1}
            query = {}
            face_list = []
            if updateAt:
                query['updateAt'] = {'$lt': updateAt}
            if user:
                query.update({'assign.user': object_id, 'assign.status': 1})
                faces = face_helper.find(query=query, keys=keys, sort={'updateAt': -1}, limit=self.PAGE_PER_COUNT)
                for face in faces:
                    media_id = face['media']
                    media = media_helper.get(media_id)
                    face['width'] = media.get('width', 0)
                    face['height'] = media.get('height', 0)
                    good = GoodHelper.find_one({'goods': face['_id']})
                    price = good['price'] if good else 0
                    face['price'] = price
                    face['updateAt'] = face.pop('updateAt').strftime('%Y-%m-%d %H:%M:%S.%f')
                    face['mosaic'] = getMosicLevel(user_id, face, media, good)
                    face['goodsId'] = good['_id'] if good else None
                    face['user'] = object_id
                    face_list.append(face)
            else:
                query.update({'backupUser': object_id})
                medias = media_helper.find(query=query, sort={'updateAt': -1}, limit=self.PAGE_PER_COUNT)
                for media in medias:
                    face_list.append({'width': media['width'], 'height': media['height'],
                                      'updateAt': media.pop('updateAt').strftime('%Y-%m-%d %H:%M:%S.%f'),
                                      'mosaic': 8, 'price': 0, 'goodsId': None, 'file': media['file'],
                                      '_id': media['_id'], 'media': media['_id'], 'user': object_id})
            result['face_list'] = face_list
            self.write(result)
            # 清空小红点提醒
            ClassHelper('StatCount').updateOne({'name': 'toClaim_' + user_id}, {"$set": {'count': 0}},
                                               upsert=True)
        elif action == "info":
            self.write(self.profile(object_id, int(isUser)))
        else:
            self.write(ERR_PATH_PERMISSION.message)

    ### 获取用户的个人主页信息
    def profile(self, userId, isUser=1):#isUser：1真实用户，0backupUser
        obj = {
            'followees': 0,
            'relationStatus': 0,
            'blackStatus': 0,
            'isUser':isUser #1为user，0为backupUser
        }
        try:
            followeeHelper = ClassHelper('Followee')
            faceHelper = ClassHelper('Face')
            if isUser:
                userInfoHelper = ClassHelper('UserInfo')
                item = userInfoHelper.find_one({"user": userId})
                obj['user'] = item['user']
                # obj["nickName"] = item.get("nickName",None)
                # obj["avatar"] = item.get("avatar", None)
                # 关注数
                followeesCount = followeeHelper.query_count({'user':userId,"effect":{"$gt":0}}) or 0
                obj['followees'] = followeesCount
                #照片数
                mediasCount = faceHelper.query_count( {'assign.user': userId,'assign.status': 1} ) or 0
                obj['medias'] = mediasCount

                #粉丝数
                followersCount = followeeHelper.query_count({'followee':userId,"effect":{"$gt":0}}) or 0
                obj['followers'] = followersCount

                #关注状态
                itemF = followeeHelper.find_one({"user":self.user['_id'],"followee":userId, "effect":{"$gt":0}})
                if itemF:#0 没有关系 1 关注 2 相互关注 3 粉丝关系
                    obj['relationStatus'] = itemF['effect']
                else:
                    itemF = followeeHelper.find_one({"followee": self.user['_id'], "user": userId})
                    if itemF:
                        obj['relationStatus'] = 3


            else:
                backupUserHelper = ClassHelper('BackupUser')
                item = backupUserHelper.find_one({"_id": userId})
                #照片数
                mediaHelper = ClassHelper( 'Media' )
                mediasCount = mediaHelper.query_count( {'backupUser': userId} )

                #粉丝数
                followersCount = followeeHelper.query_count({'backupFollowee':userId,"effect":{"$gt":0}}) or 0
                obj['followers'] = followersCount

                obj['medias'] = mediasCount
                obj['user'] = item['_id']

                #关注状态
                itemF = followeeHelper.find_one({"user":self.user['_id'],"backupFollowee":userId, "effect":{"$gt":0}})
                if itemF:#0 没有关系 1 关注 2 相互关注 3 粉丝关系
                    obj['relationStatus'] = itemF['effect']
                else:
                    itemF = followeeHelper.find_one({"backupFollowee": self.user['_id'], "user": userId})
                    if itemF:
                        obj['relationStatus'] = 3



            if item:
                obj["nickname"] = item.get("nickName", None)
                obj["avatar"] = item.get("editAvatar", None) or item.get("avatar", None)
                obj["age"] = item.get("age",0)
                obj["address"] = item.get("address",None)
                obj["gender"] = item.get("gender",None)
                rect = item.get("editRect",None) or item.get("rect",None)
                if rect:
                    rect = [int(i) for i in rect]
                obj['rect'] = rect
                obj["_id"] = item['_id']

            #贡献者数
            assignersCount = faceHelper.distinct({'assign.user':userId,'assign.status': 1},"assign.assigner") or []
            obj['assigners'] = len(assignersCount.cursor)
            #照片数
            # mediasCount = faceHelper.query_count({'assign.user':userId}) or 0
            # obj['mediasCount'] = mediasCount
            if self.user["_id"] != userId:
                #拉黑状态
                blackHelper = ClassHelper('Blacklist')
                item = blackHelper.find_one({"user": self.user['_id'], "blacker": userId})
                if item:
                    obj['blackStatus'] = 1
        except Exception,ex:
            log.err(ex.message)
            return ERR_INVALID.message
        return obj



def getMosicLevel(userId, face, media, good=None, hasUser=False):
    """
    获取照片马赛克等级
    :param userId: 登录用户
    :param face: 人脸object
    :param media: 照片object
    :param good: 商品object
    :param hasUser: 照片中是否有用户
    :return:
    """
    assign = face.get('assign')
    # 我贡献的0
    if assign and assign.get('assigner') == userId:
        return 0
    # 我购买过的1
    charge_helper = ClassHelper('ChargeFlow')
    charge_record = charge_helper.find_one({'user': userId, 'goods': good['_id'], 'status': 1}) if good else None
    if charge_record:
        return 1
    if not hasUser:
        hasUser = _mediaHasUser(media, userId)
    # 照片中有我5
    if hasUser:
        return 5
    # 默认10
    return 10


def _mediaHasUser(media, user):
    """
    图片中是否有用户
    :param media:
    :param user:
    :return:
    """
    faces = media.get('faces', [])
    if not faces:
        return False
    face_helper = ClassHelper('Face')
    for faceId in faces:
        face = face_helper.get(faceId)
        assign = face.get('assign', None)
        if not assign:
            continue
        if assign.get('user', None) == user and assign.get('status') == 1:
            return True
    return False


        # obj = {
        #     'followeesCount': 0,
        #     'relationStatus': 0,
        #     'blackStatus': 0,
        #     'isUser':isUser #1为user，0为backupUser
        # }
        # try:
        #     classHelper = ClassHelper('StatCount')
        #     if isUser:
        #         userInfoHelper = ClassHelper('UserInfo')
        #         item = userInfoHelper.find_one({"user": userId})
        #         obj['user'] = item['user']
        #         # obj["nickName"] = item.get("nickName",None)
        #         # obj["avatar"] = item.get("avatar", None)
        #         # 关注数
        #         followeesCount = classHelper.find_one({'name': "followees_" + userId}) or {}
        #         obj['followeesCount'] = followeesCount.get('count', 0)
        #     else:
        #         backupUserHelper = ClassHelper('BackupUser')
        #         item = backupUserHelper.find_one({"_id": userId})
        #         obj['user'] = item['_id']
        #     if item:
        #         obj["nickName"] = item.get("nickName", None)
        #         obj["avatar"] = item.get("avatar", None)
        #         obj["age"] = item.get("age",0)
        #         obj["address"] = item.get("address",None)
        #         obj['rect'] = item.get("rect",None)
        #         obj["_id"] = item['_id']
        #     #粉丝数
        #     followersCount = classHelper.find_one({'name': "followers_" + userId}) or {}
        #     obj['followersCount'] = followersCount.get('count', 0)
        #     #贡献者数
        #     assignersCount = classHelper.find_one({'name': "assigners_" + userId}) or {}
        #     obj['assignersCount'] = assignersCount.get('count', 0)
        #     #照片数
        #     mediasCount = classHelper.find_one({'name': "medias_" + userId}) or {}
        #     obj['mediasCount'] = mediasCount.get('count', 0)
        #     if self.user["_id"] != userId:
        #         #关注状态
        #         followHelper = ClassHelper('Followee')
        #         item = followHelper.find_one({"user":self.user['_id'],"followee":userId, "effect":{"$gt":0}})
        #         if item:#0 没有关系 1 关注 2 相互关注 3 粉丝关系
        #             obj['relationStatus'] = item['effect']
        #         else:
        #             item = followHelper.find_one({"followee": self.user['_id'], "user": userId})
        #             if item:
        #                 obj['relationStatus'] = 3
        #         #拉黑状态
        #         blackHelper = ClassHelper('Blacklist')
        #         item = blackHelper.find_one({"user": self.user['_id'], "blacker": userId, "effect":{"$gt":0}})
        #         if item:
        #             obj['blackStatus'] = 1
        # except Exception,ex:
        #     log.err(ex.message)
        #     return ERR_INVALID.message
        # return obj