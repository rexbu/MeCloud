import redis
from mecloud.lib import log


class RedisDBConfig:
    HOST = None
    PORT = None
    DBID = None
    PASSWORD = None
    MAX_CONNECTIONS = None


class RedisDb():
    client = {}

    # rc = None

    @staticmethod
    def set(key, value, dbid=0):
        try:
            return RedisDb.get_connection(dbid).set(key, value)
        except Exception, e:
            log.err("redis set operation fail , error:%s", str(e))
            return False

    @staticmethod
    def get(key, dbid=0):
        try:
            return RedisDb.get_connection(dbid).get(key)
        except Exception, e:
            log.err("redis get operation fail , error:%s", str(e))
            return None

    @staticmethod
    def delete(key, dbid=0):
        try:
            return RedisDb.get_connection(dbid).delete(key)
        except Exception, e:
            log.err("redis delete operation fail , error:%s", str(e))
            return 0

    @staticmethod
    def setex(key, value, expireSeconds, dbid=0):
        try:
            return RedisDb.get_connection(dbid).setex(key, value, expireSeconds)
        except Exception, e:
            log.err("redis setex operation fail , error:%s", str(e))
            return False

    @staticmethod
    def hset(key, field, value, dbid=0):
        try:
            return RedisDb.get_connection(dbid).hset(key, field, value)
        except Exception, e:
            log.err("redis hset operation fail , error:%s", str(e))
            return -1

    @staticmethod
    def hget(key, field, dbid=0):
        try:
            return RedisDb.get_connection(dbid).hget(key, field)
        except Exception, e:
            log.err("redis hget operation fail , error:%s", str(e))
            return None

    @staticmethod
    def hgetall(key, dbid=0):
        try:
            return RedisDb.get_connection(dbid).hgetall(key)
        except Exception, e:
            log.err("redis hgetall operation fail , error:%s", str(e))
            return {}

    @staticmethod
    def expire(key, expireSeconds, dbid=0):
        try:
            return RedisDb.get_connection(dbid).expire(key, expireSeconds)
        except Exception, e:
            log.err("redis expire operation fail , error:%s", str(e))
            return False

    @staticmethod
    def incrby(key, amount=1, dbid=0):
        try:
            return RedisDb.get_connection(dbid).incr(key, amount)
        except Exception, e:
            log.err( "redis incrby operation fail , error:%s", str(e))
            return None

    @staticmethod
    def zadd(key, score, member, dbid=0):
        try:
            return RedisDb.get_connection(dbid).zadd(key, member, score)
        except Exception, e:
            log.err( "redis zadd operation fail , error:%s", str( e ) )
            return None

    @staticmethod
    def zrange(key, start, end, withscores=False, dbid=0):
        try:
            return RedisDb.get_connection( dbid ).zrange( key, start, end, withscores=withscores)
        except Exception, e:
            log.err( "redis zrangebyscore operation fail , error:%s", str( e ) )
            return None

    @staticmethod
    def zrem(key, member, dbid=0):
        try:
            return RedisDb.get_connection( dbid ).zrem( key, member )
        except Exception, e:
            log.err( "redis zadd operation fail , error:%s", str( e ) )
            return None

    @staticmethod
    def get_connection(dbid=0):
        if not RedisDb.client.has_key(dbid):
            RedisDb.client[dbid] = redis.Redis(host=RedisDBConfig.HOST, password=RedisDBConfig.PASSWORD, db=dbid)
        # if not RedisDb.rc:
        #     RedisDb.rc = redis.Redis(host=RedisDBConfig.HOST, password=RedisDBConfig.PASSWORD,db=1)
        return RedisDb.client[dbid]


if __name__ == '__main__':
    RedisDBConfig.HOST = 'testapi1.blinnnk.com'
    RedisDBConfig.PORT = 6379
    RedisDBConfig.DBID = 0
    RedisDBConfig.PASSWORD = 'a5d3923d-40a5-453a-8071-23ac5b6e2703:wee8uchienoneV6e'
    RedisDBConfig.MAX_CONNECTIONS = 10
    # print RedisDb.setex('Testkey11111', "Simple Test", 1, 0)
    # print RedisDb.delete('Testkey11111')
    # print RedisDb.hset('wyh', 2, 3)
    # print RedisDb.hset('wyh', 2, 3)
    # print RedisDb.hget('wyh', 3)
    # print RedisDb.hgetall('wyh')
    # print RedisDb.hgetall('1')
    print RedisDb.expire('wyh',1)
    print RedisDb.hget('wyh', 3)
