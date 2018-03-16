from mecloud.helper.RedisHelper import *
from mecloud.constants.RedisConstants import *
import thread

ANCHOR_SEPERATOR = "_"


def getCacheByOid(className, objectId):
    try:
        if not RedisDBConfig.HOST:
            return None
        dbId = RedisDb.hget(RedisConstants.CLASSNAME_DB_NO_MAP, className)
        if not dbId:
            return None
        value = RedisDb.get(className + ANCHOR_SEPERATOR + objectId, dbId)
        if value:
            RedisDb.expire(className + ANCHOR_SEPERATOR + objectId, RedisConstants.EXPIER_HOUR, dbId)
        return value
    except Exception, e:
        log.err("get cache by oid , error:%s", str(e))
        return None


def setCacheByOid(className, objectId, value, expierSeconds=RedisConstants.EXPIRE_MINUTE):
    try:
        if not RedisDBConfig.HOST:
            return False
        dbId = RedisDb.hget(RedisConstants.CLASSNAME_DB_NO_MAP, className)
        if not dbId:
            dbId = RedisDb.incrby(RedisConstants.CLASSNAME_MAX_DB_NO)
            RedisDb.hset(RedisConstants.CLASSNAME_DB_NO_MAP, className, dbId)
        return RedisDb.setex(className + ANCHOR_SEPERATOR + objectId, value, expierSeconds, dbId)
    except Exception, e:
        log.err("get cache by oid , error:%s", str(e))
        return False


def deleteCacheByOid(className, objectId):
    try:
        if not RedisDBConfig.HOST:
            return 0
        dbId = RedisDb.hget(RedisConstants.CLASSNAME_DB_NO_MAP, className)
        if not dbId:
            return 0
        return RedisDb.delete(className + ANCHOR_SEPERATOR + objectId, dbId)
    except Exception, e:
        log.err("get cache by oid , error:%s", str(e))
        return 0

def getCacheByCondition(className, condition):
    try:
        if not RedisDBConfig.HOST:
            return None
        dbId = RedisDb.hget(RedisConstants.CLASSNAME_DB_NO_MAP, className)
        if not dbId:
            return None
        value = RedisDb.get(condition, dbId)
        result=None
        if value:
            result=value.split(',')
            # RedisDb.expire(condition, RedisConstants.EXPIER_HOUR, dbId)
        return result
    except Exception, e:
        log.err("get cache by condition , error:%s", str(e))
        return None


def setCacheByCondition(className, condition, param, expierSeconds=RedisConstants.EXPIRE_MINUTE):
    try:
        if not RedisDBConfig.HOST:
            return False
        dbId = RedisDb.hget(RedisConstants.CLASSNAME_DB_NO_MAP, className)
        if not dbId:
            dbId = RedisDb.incrby(RedisConstants.CLASSNAME_MAX_DB_NO)
            RedisDb.hset(RedisConstants.CLASSNAME_DB_NO_MAP, className, dbId)
        value=",".join(param)
        return RedisDb.setex(condition, value, expierSeconds, dbId)
    except Exception, e:
        log.err("get cache by condition , error:%s", str(e))
        return False


def deleteCacheByCondition(className, condition,param):
    try:
        if not RedisDBConfig.HOST:
            return 0
        dbId = RedisDb.hget(RedisConstants.CLASSNAME_DB_NO_MAP, className)
        if not dbId:
            return 0
        return RedisDb.delete(className + ANCHOR_SEPERATOR + objectId, dbId)
    except Exception, e:
        log.err("get cache by condition , error:%s", str(e))
        return 0
