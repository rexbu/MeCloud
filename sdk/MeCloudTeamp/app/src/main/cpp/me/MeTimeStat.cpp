/**
 * file :	MeTimeStat.cpp
 * author :	Rex
 * create :	2016-10-31 23:11
 * func : 
 * history:
 */

#include "McDevice.h"
#include "MeTimeStat.h"

MeTimeStat::MeTimeStat(const char* name):
MeObject(name){
    m_start = 0;
    m_prevtime = 0;
    put("user", mc::device_id());
}

void MeTimeStat::start(){
    m_start = time(0);
    m_prevtime = m_start;
    m_duration = 0;
    
    put("start", (long)m_start);
    put("duration", (long)0);
}

void MeTimeStat::refresh(){
     time_t duration = time(0)-m_prevtime;
     if (duration > 10) {
         m_prevtime = time(0);
         increase("duration", (long)duration);
     }
}

void MeTimeStat::stop(){
    if (m_start<=0) {
        return;
    }
    
    m_start = 0;    // m_start=0用来标记stop
    if (time(0)-m_prevtime>0) {
        increase("duration", time(0)-m_prevtime);
        save(this);
    }
    
    clear();
    m_objectid = NULL;
}

void MeTimeStat::done(MeObject* obj, MeException* err, uint32_t size){
    if (err!=NULL) {
        // 写入本地文件
        debug_log("errCode:%d msg:%s", err->intValue("errCode"), err->stringValue("errMsg"));
    }
    else{
        // debug_log("%s: duration[%ld]", className(), obj->longValue("duration"));
    }
    // 如果已经stop，则本次请求为最后一次，需要清空
    if (m_start<=0) {
        clear();
        m_objectid = NULL;
    }
}
