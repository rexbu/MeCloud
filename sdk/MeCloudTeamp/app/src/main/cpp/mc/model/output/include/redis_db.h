/**
 * file :	redis_db.h
 * author :	bushaofeng
 * create :	2014-08-06 14:47
 * func : 
 * history:
 */

#ifndef	__REDIS_DB_H_
#define	__REDIS_DB_H_

#include <stdio.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>
#include "bs.h"
#include "db.h"
#include "basic.h"

#define SETTIME 500

class RedisDB:public DB{
    /*
protected:
    static RedisDB*     m_instance;
public:
    static RedisDB* initialize(const char* ip, int port, int db=0){
    if (m_instance==NULL){
            m_instance = new RedisDB(ip, port, db);
        }
        return m_instance;
    }
    
    static RedisDB* instance(){
        return m_instance;
    }
     */
  
public:
    RedisDB(const char* ip, int port, int db=0):DB(ip, port){
        m_redis = redisConnect(ip, port);
        if(m_redis->err){
            redisFree(m_redis);
        }
        selectDB(db);
    }
    RedisDB(const char* ip, int port, struct event_base* base, int db=0):DB(ip, port){
        m_async = redisAsyncConnect(ip, port);
        redisLibeventAttach(m_async, base);
        redisAsyncSetConnectCallback(m_async, NULL);
        redisAsyncSetDisconnectCallback(m_async, NULL);
        redisAsyncCommand(m_async, NULL, NULL, "SELECT %d", db);
    }
    
    void asyncCommand(redisCallbackFn callback, void* para, const char fmt[], ...){
        va_list             va;
        char                buf[1024];
        
        va_start(va, fmt);
        vsnprintf(buf, 1024, fmt, va);
        redisAsyncCommand(m_async, callback, para, buf);
    }

    state_t selectDB(int db){
        redisReply* r=(redisReply*)redisCommand(m_redis,"SELECT %d", db);
        if (strcmp(r->str,"OK")!=0) {
            debug_log("select database[%d] error", db);
            freeReplyObject(r);
            return BS_INVALID;
        }
        freeReplyObject(r);
        return BS_SUCCESS;
    }
    state_t exists(const char* key){
        if (key == NULL) {
            return BS_PARAERR;
        }
        state_t     st = BS_SUCCESS;
        redisReply* r=(redisReply*)redisCommand(m_redis, "EXISTS %s",key);
        if(r->integer != 1){
            st = BS_NOTFOUND;
        }
        freeReplyObject(r);
        return st;
    }
    // op为格式化命令，如“HSET %s xxx %s”
    state_t set(const char* op, const char* key, uint64_t value);
    int64_t get(const char* op, const char* key);
    state_t set(const char* op, const char* key, void* value, uint32_t size);
    int get(const char* op, const char* key, void* value, uint32_t size);
    state_t setHash(const char* key, const char* field, void* value, uint32_t size);
    int getHash(const char* key, const char* field, void* value, uint32_t size);
    state_t delHash(const char* key, const char* field);
    state_t delAll(const char* key);

    state_t setex(const char* key, uint32_t time, void* value, uint32_t size);
    state_t setpex(const char* key, uint32_t time, void* value, uint32_t size);
    
protected:
    redisContext*       m_redis;
    redisAsyncContext*  m_async;
    //aeEventLoop*        loop;
};

#endif
