#include "m3u8.h"
M3U8::M3U8() {	
    this->duration = 0;
    this->targetDuration = 0;
    vector_init(&m_ts_list);
}
M3U8::M3U8(const char* m3u8) {
    vector_init(&m_ts_list);
    this->duration = 0;
    this->targetDuration = 0;	
    assert(parse(m3u8)==BS_SUCCESS);
}
int M3U8::getTargetDuration()
{
    return targetDuration;
}
// 析构方法
M3U8::~M3U8(){
}
//释放vector
void M3U8::ts_list_destory()
{
    vector_destroy(&m_ts_list);
}
//获取m3u8的名称
const char* M3U8::getM3u8name()
{
    return m3u8name;
}
//获取总个数
int M3U8::getCount()
{
    return vector_vlen(&m_ts_list);
}
//获取总时长
double M3U8::getDuration()
{
    return duration;
}
//获取所有ts
m3u8_ts_t* M3U8::getTsList()
{
    return getTS(0);
}
//解析m3u8文件
state_t M3U8::parse(const char* m3u8file)			// 解析一个m3u8文件
{
    m3u8name = m3u8file;
    char buffer[TS_URL_SIZE];
    string buffer2;
    int length;
    fstream out;
    out.open(m3u8file,ios::in);
    if(!out)
    {
        return BS_INVALID;
    }
    while(!out.eof())
    {
        out.getline(buffer,TS_URL_SIZE,'\n');//getline(char *,int,char) 表示该行字符达到256个或遇到换行就结束
        if(!strncmp(buffer,"#EXTINF:",8))
        {
            struct m3u8_ts_t m3u8ts;
            buffer2="" ;
            length = strlen(buffer);
            for(int i =8;i<length-1;i++)//获取时长
            {
                buffer2 += buffer[i];
            } 
            m3u8ts.time = atof(buffer2.c_str());
            duration += m3u8ts.time;
            out.getline(m3u8ts.ts_url,TS_URL_SIZE,'\n');
            vector_push(&m_ts_list,&m3u8ts);
        }
        else
        {
            if(!strncmp(buffer,"#EXT-X-TARGETDURATION:",22))
            {
                targetDuration = (int)(buffer[strlen(buffer)-1]-'0');
            }
        }
    } 
    out.close();
    return BS_SUCCESS;

}
struct m3u8_ts_t* M3U8::getTS(uint32_t i)	// 获取第i个ts,如果成功返回
{
    if(i < getCount())//i必须小于ts的总个数,是否带等号,有待考察?
    {
        struct m3u8_ts_t* ts;
        ts= (m3u8_ts_t*)vector_index(&m_ts_list,i);
        return ts;
    }
    else
        return NULL;
}
/*
 * 获取从start开始的size个ts
 * 函数返回获取到的ts指针，如果错误返回NULL
 * get_size返回获取的ts数
 */
struct m3u8_ts_t* M3U8::getTS(uint32_t start, uint32_t size, uint32_t* get_size)
{
    if(start >= getCount())
        return NULL;
    else
    {
        uint32_t count = 0;
        struct m3u8_ts_t* ts; 
        while(count < size)
        {
            ts = getTS(start+count);
            if(ts == NULL)
            {
                break;
            }
            count++;
        }
        *get_size = count;
        return getTS(start);
    }
}
