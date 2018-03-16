#ifndef __M3U8_H_
#define __M3U8_H_

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>  
#include <stdlib.h>  
#include <math.h> 
#include "bs.h"

#define TS_URL_SIZE 256
using namespace std;

//存储ts的相关信息,ts的时长和地址
struct m3u8_ts_t{
    char ts_url[TS_URL_SIZE];
    double time; 
};

class M3U8{
    vector_t(m3u8_ts_t)	m_ts_list;//ts信息
    const char* m3u8name;
    double duration;//ts文件的总时长
    int targetDuration;
public:
    //构造方法
    M3U8();
    M3U8(const char* m3u8);
    int getTargetDuration();
    // 析构方法
    ~M3U8();
    const char* getM3u8name();
    //获取总个数
    int getCount();
    //获取总时长
    double getDuration();
    //获取所有ts
    m3u8_ts_t* getTsList();
    //解析m3u8文件
    state_t parse(const char* m3u8file);
    struct m3u8_ts_t* getTS(uint32_t i);	// 获取第i个ts,如果成功返回
    /*
     * 获取从start开始的size个ts
     * 函数返回获取到的ts指针，如果错误返回NULL
     * get_size返回获取的ts数
     */
    struct m3u8_ts_t* getTS(uint32_t start, uint32_t size, uint32_t* get_size);
    void ts_list_destory();
};

#endif
