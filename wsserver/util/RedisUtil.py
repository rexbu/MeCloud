# coding=utf-8
from mecloud.helper.RedisHelper import RedisDb


# USER_COOKIE_RECORD_PRE = 'ucr_'

def setCookie(cookie, uid):
    return RedisDb.setex(cookie, uid, 3600 * 24 * 30)


def getUid(cookie):
    return RedisDb.get(cookie)
