# -*- coding: utf-8 -*-
from mecloud.helper.ClassHelper import ClassHelper
from mecloud.lib import log
from mecloud.model.MeError import ERR_INVALID


def getIFCCount(userId=None, backupUser=None, keys=None):
    '''
    # 粉丝数：Followee (backup)followee, 照片数(认领的照片)：Face assign.user,贡献者个数：Face assign.assigner uploader
    :param userId: 注册用户Id
    :param backupUser: 非注册用户Id
    :param keys: 查询某一个Count
    :return: 
    '''
    uid = userId or backupUser
    isUser = bool(userId)
    condition = {
        'imageCount': {'classname': 'Face', 'key': 'imageCount', 'condition': {'assign.status': 1, 'assign.user': uid},
                       'method': 1},
        'fansCount': {'classname': 'Followee', 'key': 'fansCount', 'condition': {'followee': uid, 'effect': 1},
                      'method': 1},
        'contributeCount': {'classname': 'Face', 'distinct': 'assign.assigner', 'key': 'contributeCount',
                            'condition': {'assign.status': 1, 'assign.user': uid}, 'method': 2}
    }
    where = []
    if not keys:
        if isUser:
            keys = {'imageCount': 1, 'fansCount': 1, 'contributeCount': 1}
        else:
            keys = {'fansCount': 1}
    for key in keys:
        where.append(condition[key])

    result = {}
    for item in where:  # 获取三种个数
        className = item['classname']
        query = item["condition"]
        try:
            if item['method'] == 1:
                classHelper = ClassHelper(className)
                result[item['key']] = classHelper.query_count(query)
            elif item['method'] == 2:
                classHelper = ClassHelper(className)
                obj = classHelper.distinct(query, item['distinct'])
                result[item['key']] = len(obj)
        except Exception, e:
            log.err("%s param error", item['key'])
    if result:
        return result
    else:
        return None


def get_specific_count(userid):
    '''
    :param userid:
    :return:{}

    followers 粉丝
    followees 关注的人
    uploaders 贡献的照片数量
    medias 拥有的照片数量
    faces 拥有的脸的数量
    assigners 有多少人贡献照片
    '''
    l = ['uploaders', 'faces']
    result = {}
    for i in l:
        result[i] = 0
    stat_unique_helper = ClassHelper('StatCount')
    query = {'$or': [{'name': l[0] + '_' + userid}, {'name': l[1] + '_' + userid}]}
    objs = stat_unique_helper.find(query)
    # print 'objs:', objs
    if objs:
        for obj in objs:
            for i in l:
                if obj['name'] == i + '_' + userid:
                    result[i] = obj['count']
                    break
    # print 'result:', result

    obj = {}
    followeeHelper = ClassHelper( 'Followee' )
    buserHelper = ClassHelper( 'BackupUser' )
    faceHelper = ClassHelper( 'Face' )

    isUser=True
    findUser = buserHelper.find_one( {'_sid': userid} )
    if findUser:
        isUser=False

    if isUser:
        # 关注数
        followeesCount = followeeHelper.query_count( {'user': userid, "effect": {"$gt": 0}} ) or 0
        obj['followees'] = followeesCount
        mediasCount = faceHelper.query_count( {'assign.user': userid, 'assign.status': 1} ) or 0
        obj['medias'] = mediasCount
        # 粉丝数
        followersCount = followeeHelper.query_count( {'followee': userid, "effect": {"$gt": 0}} ) or 0
        obj['followers'] = followersCount
    else:
        mediaHelper = ClassHelper( 'Media' )
        mediasCount = mediaHelper.query_count( {'backupUser': userid} )
        obj['medias'] = mediasCount
        # 粉丝数
        followersCount = followeeHelper.query_count( {'backupFollowee': userid, "effect": {"$gt": 0}} ) or 0
        obj['followers'] = followersCount

    # 贡献者数
    assignersCount = faceHelper.distinct( {'assign.user': userid, 'assign.status': 1}, "assign.assigner" ) or []
    obj['assigners'] = len( assignersCount.cursor )

    count_key = obj.keys()
    for key in count_key:
        result[key] = obj[key]

    return result



def profile(userId, isUser=1):#isUser：1真实用户，0backupUser
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
                # itemF = followeeHelper.find_one({"user":self.user['_id'],"followee":userId, "effect":{"$gt":0}})
                # if itemF:#0 没有关系 1 关注 2 相互关注 3 粉丝关系
                #     obj['relationStatus'] = itemF['effect']
                # else:
                #     itemF = followeeHelper.find_one({"followee": self.user['_id'], "user": userId})
                #     if itemF:
                #         obj['relationStatus'] = 3


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
                # itemF = followeeHelper.find_one({"user":self.user['_id'],"backupFollowee":userId, "effect":{"$gt":0}})
                # if itemF:#0 没有关系 1 关注 2 相互关注 3 粉丝关系
                #     obj['relationStatus'] = itemF['effect']
                # else:
                #     itemF = followeeHelper.find_one({"backupFollowee": self.user['_id'], "user": userId})
                #     if itemF:
                #         obj['relationStatus'] = 3



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

            # if self.user["_id"] != userId:
            #     #拉黑状态
            #     blackHelper = ClassHelper('Blacklist')
            #     item = blackHelper.find_one({"user": self.user['_id'], "blacker": userId})
            #     if item:
            #         obj['blackStatus'] = 1
        except Exception,ex:
            log.err(ex.message)
            return ERR_INVALID.message
        return obj