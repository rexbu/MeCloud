#ifndef __M3U8CLIP_H_
#define __M3U8CLIP_H_

#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <string.h>
#include <stdio.h>  
#include <stdlib.h>  
#include <math.h> 
#include "bs.h"
#include "m3u8.h"

using namespace std;

#define M3U8_CLIP_SIZE 1024

struct ts_flag_t{
    uint8_t			flag:1;		// 该ts是否被选中
    uint32_t		index:31;	// 该ts在m3u8中是第几个ts
    M3U8			m3u8;		// m3u8指针
};

class M3U8Clip{
    list<M3U8> m_m3u8_list;//M3U8文件列表
    list<M3U8>::iterator iter;

    list<ts_flag_t> m_ts_flag;//所有ts列表
    list<ts_flag_t>::iterator its;
    int targetDuration;
public:
    M3U8Clip();
    ~M3U8Clip();
    // 加入一个m3u8文件
    state_t add(const char* m3u8file);	
    // 在位置i处添加一个m3u8文件		
    state_t add(const char* m3u8file, uint32_t i);
    // 删除一个m3u8文件	
    state_t remove(const char* m3u8file);
    // 删除第i个m3u8文件		
    state_t remove(uint32_t i); //参数为0时
    // 添加一个ts，i是在所有ts中的序号						
    state_t selectTS(uint32_t i);
    // 从start开始，选中size个ts，start是在所有ts中的序号					
    state_t selectTS(int start, int size);
    //添加m3u8中第i个ts	
    state_t selectTS(const char* m3u8file, uint32_t i);
    //添加m3u8中个从start开始共size个ts	
    state_t selectTS(const char* m3u8file, uint32_t start, uint32_t size); 
    state_t removeTS(uint32_t i);	
    state_t removeTS(int start, int size);
    state_t removeTS(const char* m3u8file, uint32_t i);
    state_t removeTS(const char* m3u8file, uint32_t start, uint32_t size);
    // 将选中的ts生成一个新的m3u8，ts需要重新拷贝出一份
    state_t build(const char* bath);
    //文件拷贝
    state_t CopyFile(char *SourceFile,const char *NewFile);
    //获取m3u8文件名
    const char* getname();
};

#endif
