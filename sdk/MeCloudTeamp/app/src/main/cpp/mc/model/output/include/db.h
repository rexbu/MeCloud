/**
 * file :	db.h
 * author :	bushaofeng
 * create :	2014-08-06 14:33
 * func : 
 * history:
 */

#ifndef	__DB_H_
#define	__DB_H_

#include "bs.h"

class DB{
    public:
        DB(const char* ip, int port){
            bs_strcpy(this->ip, IP_SIZE, ip);
            this->port = port;
        }

        // 写数据
        virtual state_t set(const char* op, const char* key, void* value, uint32_t size) = 0;
        // 读数据
        virtual int get(const char* op, const char* key, void* value, uint32_t size) = 0;

        // 设置数据并设置过期时间， 单位秒
        virtual state_t setex(const char* key, uint32_t time, void* value, uint32_t size) = 0;
        // 设置数据并设置过期时间，单位毫秒
        virtual state_t setpex(const char* key, uint32_t time, void* value, uint32_t size) = 0;
 

    protected:
        char        ip[IP_SIZE];
        int         port;
};

#endif
