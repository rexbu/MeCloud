#-*- coding: utf-8 -*- 
'''
 * file :	redis.py
 * author :	Rex
 * create :	2018-07-28 15:39
 * func : 
 * history:
'''
import redis, traceback
from mecloud.lib import *

class Redis:
    client = {}
    host = 'localhost'
    password = None
    port = 6379
    pool = None

    @staticmethod
    def init(host, pwd=None, port=6379):
        Redis.host = host
        Redis.password = pwd
        Redis.port = port
        
        #pool = redis.ConnectionPool(host=host, password=pwd, port=port)
    @staticmethod
    def conn(dbid):
        if not Redis.client.has_key(dbid):
            '''
            if Redis.pool:
                Redis.client[dbid] = redis.Redis(connection_pool=pool, db=dbid)
            else:
            '''
            Redis.client[dbid] = redis.Redis(host=Redis.host, password=Redis.password, port=Redis.port, db=dbid)
        return Redis.client[dbid]

    @staticmethod
    def destroy():
        for c in Redis.client:
            c.close()

    def __init__(self, dbid=0):
        if not Redis.client.has_key(dbid):
            '''
            if Redis.pool:
                Redis.client[dbid] = redis.Redis(connection_pool=pool, db=dbid)
            else:
            '''
            Redis.client[dbid] = redis.Redis(host=Redis.host, password=Redis.password, port=Redis.port, db=dbid)

        self.db = Redis.client[dbid]
    
    def set(self, key, value):
        try:
            return self.db.set(key, value)
        except Exception, e:
            log.err("redis set operation fail , error:%s", str(e))
            return False

    def get(self, key):
        try:
            return self.db.get(key)
        except Exception, e:
            log.err("redis get operation fail , error:%s", str(e))
            return None

    def delete(self, key, dbid=0):
        try:
            return self.db.delete(key)
        except Exception, e:
            log.warn("redis delete operation fail , error:%s", str(e))
            return 0

    def setex(self, key, value, expireSeconds):
        try:
            return self.db.setex(key, value, expireSeconds)
        except Exception, e:
            log.err("redis setex operation fail , error:%s", str(e))
            return False

    def hset(self, key, field, value):
        try:
            return self.db.hset(key, field, value)
        except Exception, e:
            log.err("redis hset operation fail , error:%s", str(e))
            return -1

    def hget(self, key, field):
        try:
            return self.db.hget(key, field)
        except Exception, e:
            log.err("redis hget operation fail , error:%s", str(e))
            return None

    def hgetall(self, key):
        try:
            return self.db.hgetall(key)
        except Exception, e:
            log.err("redis hgetall operation fail , error:%s", str(e))
            return None

    def expire(self, key, expireSeconds):
        try:
            return self.db.expire(key, expireSeconds)
        except Exception, e:
            log.err("redis expire operation fail , error:%s", str(e))
            return False

    def incrby(self, key, amount=1):
        try:
            return self.db.incr(key, amount)
        except Exception, e:
            log.err( "redis incrby operation fail , error:%s", str(e))
            return None
    
    # 向键为name的zset中添加元素member，score用于排序。如果该元素存在，则更新其顺序
    def zadd(self, key, score, member):
        try:
            return self.db.zadd(key, member, score)
        except Exception, e:
            log.err( "redis zadd operation fail , error:%s", str( e ) )
            return None

    def zrange(self, key, start, end, withscores=False):
        try:
            return self.db.zrange(key, start, end, withscores=withscores)
        except Exception, e:
            log.err( "redis zrangebyscore operation fail , error:%s", str( e ) )
            return None
    # 从键为name的集合中删除元素
    def zrem(self, key, member):
        try:
            return self.db.zrem(key, member )
        except Exception, e:
            log.err( "redis zadd operation fail , error:%s", str( e ) )
            return None
    # 向键为name的集合中添加元素
    def sadd(self, name, member):
            self.db.sadd(name, member)
    
    def sgetall(self, name):
        try:
            return self.db.smembers(name)
        except Exception, e:
            log.err('redis sgetall %s error: %s', name, str(e))
            return None

    def scan(self, start, num):
        cursor = 0
        try:
            if start>0:
                cursor, data = self.db.scan(cursor = cursor, count=start)
                if data==None or len(data)<start:
                    return None
            cursor, data = self.db.scan(cursor = cursor, count=num)
            return data
        except Exception,e:
            log.err("redis scan %d/%d error: %s", start, num, str(e))
            return None

    # 删除所有数据	
    def drop(self):
        keys = self.db.keys()
        self.db.delete(*keys)

    def exists(self, key):
        return self.db.exists(key)
