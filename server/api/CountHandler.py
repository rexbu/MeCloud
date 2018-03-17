#-*- coding: utf-8 -*-
from mecloud.helper.ClassHelper import ClassHelper
from mecloud.lib import log


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

def get_follow_ount(userId, isUser=1):
    '''
    # 粉丝数：Followee (backup)followee, 照片数(认领的照片)：Face assign.user,贡献者个数：Face assign.assigner uploader
    :param userId: 注册用户Id
    :return:
    '''

    # classHelper = ClassHelper( 'StatCount' )
    # try:
    #     if is_mine:
    #         result = classHelper.find( {'name': {
    #             '$in': ['albumMost_' + userId, 'albumAnimal_' + userId, 'albumBaby_' + userId,
    #                     'albumChowhound_' + userId, 'followers_' + userId, 'medias_' + userId, 'followees_' + userId,
    #                     'uploaders_' + userId, 'faces_' + userId, 'assigners_' + userId]}}, {"_id": 0} )
    #     else:
    #         result = classHelper.find( {'name': {
    #             '$in': ['followers_' + userId, 'medias_' + userId, 'followees_' + userId, 'assigners_' + userId]}},
    #                                    {"_id": 0} )
    # except Exception, e:
    #     log.err( "%s find StatCount error", userId )
    # if result:
    #     return result
    # else:
    #     return None
    #
    obj = {}
    try:
        followeeHelper = ClassHelper( 'Followee' )
        faceHelper = ClassHelper( 'Face' )
        if isUser:
            # 关注数
            followeesCount = followeeHelper.query_count( {'user': userId, "effect": {"$gt": 0}} ) or 0
            obj['followees'] = followeesCount
            mediasCount = faceHelper.query_count( {'assign.user': userId,'assign.status': 1} ) or 0
            obj['medias'] = mediasCount
            # 粉丝数
            followersCount = followeeHelper.query_count( {'followee': userId, "effect": {"$gt": 0}} ) or 0
            obj['followers'] = followersCount
        else:
            mediaHelper = ClassHelper( 'Media' )
            mediasCount = mediaHelper.query_count( {'backupUser': userId} )
            obj['medias'] = mediasCount
            # 粉丝数
            followersCount = followeeHelper.query_count( {'backupFollowee': userId, "effect": {"$gt": 0}} ) or 0
            obj['followers'] = followersCount

        # 贡献者数
        assignersCount = faceHelper.distinct( {'assign.user': userId, 'assign.status': 1}, "assign.assigner" ) or []
        obj['assigners'] = len( assignersCount.cursor )

    except Exception, ex:
        log.err( ex.message )
        return None
    return obj
