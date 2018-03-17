#-*- coding: utf-8 -*- 
'''
 * file :	FollowHelper.py
 * author :	Rex
 * create :	2017-10-27 14:24
 * func : 
 * history:
'''
import isodate
from mecloud.helper.ClassHelper import ClassHelper, ERR_PARA
from mecloud.helper.FeedHelper import addFeedRecord
from mecloud.lib import log

class FollowHelper:
    """
    user关注followee
	@ user: 发起关注动作的user
	@ followee: 被关注
	@ is_user: true:followee是User, false: followee是BackupUser
    """

    isOnline = False
    classame="Followee"
    #followHelper = ClassHelper("Followee")
    @staticmethod
    def follow(user, followee, is_user=True):
        if not user or not followee:
            log.err( "FollowHelper-->follow userId:%s, followeeId %s", user, followee)
            return
        if is_user:
            fieldname = "followee"
        else:
            fieldname = "backupFollowee"

        # 查找followee是否关注user, 如果关注则需要添加双向关注关系
        followHelper = ClassHelper(FollowHelper.classame)
        effect=1
        if is_user:
            result = followHelper.find_one({'user': followee, fieldname: user, 'effect': {'$gte': 1}})
            if not result:
                effect = 1
            else:
                effect = 2
        # 添加feed记录,第一次关注才添加
        # followRecord = followHelper.find_one({'user': user, fieldname: followee, 'effect': {'$ne': 0}})
        # if not followRecord:
        #     addFeedRecord(user, 'follow', followRecord['_id'])
        #TODO: 这里事务的管理不太严格，应该关注和粉丝统一起来
		# 添加我的关注
        followee_action = {'destClass':'StatCount', 'query': {'name': 'followees_'+user}, 'action':{'@inc':{'count':1}}}
        follower_action = {'destClass':'StatCount', 'query': {'name': 'followers_'+followee}, 'action':{'@inc':{'count': 1}} }
        actions = [follower_action, followee_action]

        followHelper.updateOne({'user': user, fieldname: followee}, {'$set': {'effect': effect, 'acl':{user: {"write": 'true'},"*": {"read": 'true'}}}}, transactions=actions, upsert=True)

        # 添加followee也是user的粉丝，则需要处理followee的双向关注，否则只添加粉丝数量
        if effect==2:
            followHelper.updateOne({'user': followee, fieldname: user}, {"$set": {'effect': effect,'updateAt':result['updateAt']}})

    """
	user取消对followee的关注
    """
    @staticmethod
    def unfollow(user, followee, is_user=True):
        if not user or not followee:
            log.err( "FollowHelper-->unfollow userId:%s, followeeId %s", user, followee)
            return;
        if is_user:
            fieldname = "followee"
        else:
            fieldname = "backupFollowee"

        followHelper = ClassHelper(FollowHelper.classame)
        result = followHelper.find_one({'user': followee, 'followee': user, 'effect': {'$gte': 2}})
        if result:
            #双向关注,需要处理followee的双向关注effect和粉丝数量
            followHelper.updateOne( {'user': followee, fieldname: user}, {"$set": {'effect': 1,'updateAt':result['updateAt']}} )

        # 取消我的关注
        followee_action = {'destClass': 'StatCount', 'query': {'name': 'followees_' + user},
                           'action': {'@inc': {'count': -1}}}
        follower_action = {'destClass': 'StatCount', 'query': {'name': 'followers_' + followee},
                           'action': {'@inc': {'count': -1}}}
        actions = [follower_action, followee_action]
        followHelper.updateOne( {'user': user, fieldname: followee}, {"$set": {'effect': 0}},
                                transactions=actions, upsert=True )

    """
    获取user的粉丝列表
    @ start: 从哪个id开始算，用于分页
    @ count: 获取的数量
    @ isuser: user是user还是BackupUser
    """
    @staticmethod
    def getFollowers(user, lastTime=None, count=10, is_user=True,skipNum=0):
        if not user:
            log.err("FollowHelper-->getFollowers userId is None")
            return None
        #if is_user:
        #    fieldname = "followee"
        #else:
        #    fieldname = "backupFollowee"

        #followHelper = ClassHelper(FollowHelper.classame)
        #sortDict = {"_sid": 1}
        #query = {fieldname:user, "_sid": {"$gt": start}, 'effect':{'$gte':1}}
        #return followHelper.find(query, sort=sortDict, limit=count)

        return FollowHelper.find_list( "Follower", user, lastTime, count, is_user, skipNum)

    """
    获取user的关注列表，发起关注的只能是User，所以不用区分是User还是BackupUser
    @ start: 从哪个id开始算，用于分页
    @ count: 获取的数量
    """
    @staticmethod
    def getFollowees(user, lastTime=None, count=10, skipNum=0):
        if not user:
            log.err( "FollowHelper-->getFollowees userId is None")
            return None

        #sortDict = {"_sid": 1}
        #query = {'user':user, "_sid": {"$gt": start}, 'effect':{'$gte':1}}
        #followHelper = ClassHelper( FollowHelper.classame )
        #return followHelper.find( query, sort=sortDict, limit=count)
        return FollowHelper.find_list("Followee", user, lastTime, count, skipNum)

    @staticmethod
    def find_list(followType, userId, lastTime=None, count=10, is_user=True, skipNum = 0):
        classHelper = ClassHelper(FollowHelper.classame)
        query = []
        matchDict = {}
        matchEffect = {}
        backDict = {}
        infoDict = {}
        noUserDict = {}

        if followType == "Followee":  # 关注   # == & is
            matchDict = {"$match": {"user": userId}}
            matchEffect = {"$match": {'effect': {'$gte': 1}}}
            backDict = {"$lookup": {
                "from": "Followee",
                "localField": "followee",
                "foreignField": "user",
                "as": "status"
            }}
            infoDict = {"$lookup": {
                "from": "UserInfo",
                "localField": "followee",
                "foreignField": "user",
                "as": "UserInfo"
            }}
            noUserDict = {"$lookup": {
                "from": "BackupUser",
                "localField": "backupFollowee",
                "foreignField": "_sid",
                "as": "backupInfo"
            }}
        elif followType == "Follower":  # 粉丝
            if is_user:
                fieldname = "followee"
            else:
                fieldname = "backupFollowee"

            matchDict = {"$match": {fieldname: userId}}
            matchEffect = {"$match": {'effect': {'$gte': 1}}}
            backDict = {"$lookup": {
                "from": "Followee",
                "localField": "user",
                "foreignField": "followee",
                "as": "status"
            }}
            infoDict = {"$lookup": {
                "from": "UserInfo",
                "localField": "user",
                "foreignField": "user",
                "as": "UserInfo"
            }}
        else:
            log.err( "param error,%s",ERR_PARA.message )
            return None

        sortDict = {"$sort": {"updateAt": -1}}  # sort is fixed to -1
        startidDict = {}
        if lastTime:
            startidDict = {"$match": {'updateAt':{"$lt": lastTime}}}

        skipDict = {"$skip": skipNum}

        if followType == "Followee":  # == & is
            limit = min( count, 100 )
        elif followType == "Follower":
            limit = min( count, 20 )
        limitDict = {"$limit": limit}

        displayDict = {'$project':
        {
                'status': 1, 'backupInfo.nickName': 1, 'backupInfo._id': 1,'backupInfo.rect': 1,'backupInfo.editRect': 1,'backupInfo.editAvatar': 1, 'backupInfo.avatar': 1,
                'UserInfo.nickName': 1, 'UserInfo.user': 1, 'UserInfo._id': 1, 'UserInfo.avatar': 1, 'user': 1,
                'followee': 1, 'backupFollowee': 1, "updateAt": 1,"effect": 1
            }
        }

        try:
            query.append( matchDict )
            query.append( matchEffect )
            query.append( sortDict )
            if startidDict != {}:
                query.append( startidDict )
            query.append( limitDict )
            query.append( backDict )
            query.append( infoDict )
            query.append( skipDict )
            if noUserDict:
                query.append( noUserDict )
            query.append( displayDict )

            followList = classHelper.aggregate( query )

            if followList is not None:
                for num in range( len( followList ) ):

                    followList[num].pop( 'status', None )
                    try:
                        if followList[num]["UserInfo"]:
                            followList[num]["UserInfo"] = followList[num]["UserInfo"][0]
                            followList[num]["UserInfo"].pop( "acl", None )
                            followList[num]["UserInfo"].pop( "createAt", None )
                            #userInput = followList[num]["UserInfo"]["user"]
                            #followList[num].update( classHelper.getIFCCount( userId=userInput ) )
                        else:
                            followList[num]["UserInfo"] = {}
                        if "backupInfo" in followList[num] and followList[num]["backupInfo"]:
                            followList[num]["backupInfo"] = followList[num]["backupInfo"][0]
                            followList[num]["backupInfo"].pop( "acl", None )
                            followList[num]["backupInfo"].pop( "createAt", None )
                            #userInput = followList[num]["backupInfo"]["_id"]
                            #followList[num].update( classHelper.getIFCCount( backupUser=userInput ) )
                        else:
                            followList[num]["backupInfo"] = {}
                    except Exception, ex:
                        log.err( "FollowerHandler Err: %s", ex )
                        return None
            else:
                return None
            # final data for querying
            return followList

        except Exception, ex:
            return None