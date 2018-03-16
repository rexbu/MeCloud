/**
 * file :	McTimer.cpp
 * author :	Rex
 * create :	2017-04-01 14:47
 * func : 
 * history:
 */

#include "McTimer.h"

using namespace mc;

Timer::Timer(long utime, void* (*callback)(void*), void* para, timer_type_t type){
    m_utime = utime;
    m_callback = callback;
    m_timer_type = type;
    m_para = para;
}

void Timer::loop(){
    switch (m_timer_type) {
        case TIMER_SLEEP:
            usleep(m_utime);
            break;
            
        case TIMER_SELECT:{
            struct timeval val;
            BS_SET_TIMEVAL(&val, m_utime);
            select(0, NULL, NULL, NULL, &val);
        }
            break;
            
        case TIMER_RTC:
            break;
            
        default:
            break;
    }
    
    m_callback(m_para);
}
