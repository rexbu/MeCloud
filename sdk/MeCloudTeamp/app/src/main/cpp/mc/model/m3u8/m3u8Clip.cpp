#include "m3u8Clip.h"

M3U8Clip::M3U8Clip()
{
    this->targetDuration = 0;
}

M3U8Clip::~M3U8Clip()
{
    for(iter = m_m3u8_list.begin(); iter != m_m3u8_list.end(); ++iter) 
    {
        iter->ts_list_destory();
    }
}

// 加入一个m3u8文件
state_t M3U8Clip::add(const char* m3u8file)
{
    M3U8 m3u8;
    m3u8.parse(m3u8file);
    for(uint32_t i =0; i<m3u8.getCount();i++)
    {
        struct ts_flag_t ts;
        ts.flag = 0;
        ts.index = i;//从零开始计数
        ts.m3u8 = m3u8;
        m_ts_flag.push_back(ts);
    } 
    if(m3u8.getTargetDuration() > targetDuration)
        targetDuration=m3u8.getTargetDuration();
    m_m3u8_list.push_back(m3u8);
    return BS_SUCCESS; 
}

// 在位置i处添加一个m3u8文件,添加成功返回0,否则返回1			
state_t M3U8Clip::add(const char* m3u8file, uint32_t i)
{
    if(i>m_m3u8_list.size())
        return BS_INVALID;
    M3U8 m3u8;
    m3u8.parse(m3u8file);
    iter = m_m3u8_list.begin();
    its = m_ts_flag.begin();
    while(i>0)
    {
        uint32_t k =0;
        //cout<<"iter->getCount:"<<iter->getCount();
        while(k < iter->getCount())
        {
            its++;
            k++;
        }
        iter++;
        i--;
    }
    for(uint32_t i =0; i<m3u8.getCount();i++)
    {
        struct ts_flag_t ts;
        ts.flag = 0;
        ts.index = i;//从零开始计数
        ts.m3u8 = m3u8;
        m_ts_flag.insert(its,ts);
    }
    if(m3u8.getTargetDuration() > targetDuration)
        targetDuration=m3u8.getTargetDuration();
    m_m3u8_list.insert(iter,m3u8);
    return BS_SUCCESS;
}

// 删除一个m3u8文件	
state_t M3U8Clip::remove(const char* m3u8file)
{
    //pts为删除的起点
    list<ts_flag_t>::iterator pts;
    its = m_ts_flag.begin();
    for(iter = m_m3u8_list.begin(); iter != m_m3u8_list.end(); ++iter)
    {
        uint32_t k =0;
        if(strcmp(m3u8file,iter->getM3u8name()) == 0)
        {
            k=iter->getCount();
            pts = its;
            while(k > 0)
            {

                its++;
                k--;
            }
            //删除ts列表中对应的ts
            m_ts_flag.erase(pts,its);
            //删除m3u8文件
            m_m3u8_list.erase(iter);
            break;
        }
        else
        {
            while(k < iter->getCount())
            {
                its++;
                k++;
            }
        }
    }
    //如果没有此m3u8文件,删除失败
    if(iter == m_m3u8_list.end())
        return BS_INVALID;
    return BS_SUCCESS;
}

// 删除第i个m3u8文件	,list中从零计数,删除成功返回0,否则返回1			
state_t M3U8Clip::remove(uint32_t i)
{
    if(i >= m_m3u8_list.size())
        return BS_INVALID;
    list<ts_flag_t>::iterator pts;
    iter = m_m3u8_list.begin();
    its = m_ts_flag.begin();
    uint32_t k =0;
    while(i>0)
    {
        while(k < iter->getCount())
        {
            its++;
            k++;
        }
        iter++;
        i--;
    }
    pts = its;
    k=0;
    while(k < iter->getCount())
    {
        its++;
        k++;
    }
    //删除ts列表中对应的ts
    m_ts_flag.erase(pts,its);
    //删除m3u8文件
    m_m3u8_list.erase(iter);
    return BS_SUCCESS;
}

// 选中一个ts，i是在所有ts中的序号,从零开始计数						
state_t M3U8Clip::selectTS(uint32_t i)
{
    if(i >= m_ts_flag.size())
        return BS_INVALID;
    its = m_ts_flag.begin();
    while(i>0)
    {
        its++;
        i--;
    }
    its->flag =1;
    return BS_SUCCESS;
}
// 从start开始，选中size个ts，start是在所有ts中的序号					
state_t M3U8Clip::selectTS(int start, int size)
{
    if(start >= m_ts_flag.size())
        return BS_INVALID;
    //保证start+size小于总长度
    //if()
    size = size > (m_ts_flag.size()-start) ? (m_ts_flag.size()-start) : size;
    its = m_ts_flag.begin();
    while (start>0)
    {
        its++;
        start--;
    }
    while (size>0)
    {
        its->flag =1;
        size--;
        its++;
    }
    return BS_SUCCESS;
}

//选中m3u8中第i个ts	
state_t M3U8Clip::selectTS(const char* m3u8file, uint32_t i)
{
    its = m_ts_flag.begin();
    for(iter = m_m3u8_list.begin(); iter != m_m3u8_list.end(); ++iter)
    {
        uint32_t k =0;
        if(strcmp(m3u8file,iter->getM3u8name()) == 0)
        {
            //如果i大于m3u8文件中ts的个数,则无法选中
            if(i >= iter->getCount())
                return BS_INVALID;
            k=i;
            //its++;
            while(k > 0)
            {

                its++;
                k--;
            }
            its->flag = 1;
            break;
        }
        else
        {
            while(k < iter->getCount())
            {
                its++;
                k++;
            }
        }
    }
    //如果没有此m3u8文件,选中失败
    if(iter == m_m3u8_list.end())
        return BS_INVALID;
    return BS_SUCCESS;
}

//添加m3u8中个从start开始共size个ts	
state_t M3U8Clip::selectTS(const char* m3u8file, uint32_t start, uint32_t size)
{
    its = m_ts_flag.begin();
    for(iter = m_m3u8_list.begin(); iter != m_m3u8_list.end(); ++iter)
    {
        uint32_t k =0;
        if(strcmp(m3u8file,iter->getM3u8name()) == 0)
        {
            //如果i大于m3u8文件中ts的个数,则无法选中
            if(start >= iter->getCount())
                return BS_INVALID;
            //k=i;
            //its++;
            //保证开始位置小于总长度
            if(start >= iter->getCount())
                return BS_INVALID;
            //保证start+size小于总长度
            size = size > (iter->getCount()-start)?(iter->getCount()-start):size;

            while(start>0)
            {
                its++;
                start--;
            }
            while(size>0)
            {
                its->flag =1;
                size--;
                its++;
            }
            break;
        }
        else
        {
            while(k < iter->getCount())
            {
                its++;
                k++;
            }
        }
    }
    //如果没有此m3u8文件,选中失败
    if(iter == m_m3u8_list.end())
        return BS_INVALID;
    return BS_SUCCESS;
}

state_t M3U8Clip::removeTS(uint32_t i)
{
    if(i >= m_ts_flag.size())
        return BS_INVALID;
    its = m_ts_flag.begin();
    while(i>0)
    {
        its++;
        i--;
    }
    its->flag = 0;
    return BS_SUCCESS;
}

state_t M3U8Clip::removeTS(int start, int size)
{
    if(start >= m_ts_flag.size())
        return BS_INVALID;
    //保证start+size小于总长度
    //if()
    size = size > (m_ts_flag.size()-start)?(m_ts_flag.size()-start):size;
    its = m_ts_flag.begin();
    while(start>0)
    {
        its++;
        start--;
    }
    while(size>0)
    {
        its->flag =0;
        size--;
        its++;
    }
    return BS_SUCCESS;
}

state_t M3U8Clip::removeTS(const char* m3u8file, uint32_t i)
{
    its = m_ts_flag.begin();
    for(iter = m_m3u8_list.begin(); iter != m_m3u8_list.end(); ++iter)
    {
        uint32_t k =0;
        if(strcmp(m3u8file,iter->getM3u8name()) == 0)
        {
            //如果i大于m3u8文件中ts的个数,则无法选中
            if(i >= iter->getCount())
                return BS_INVALID;
            k=i;
            //its++;
            while(k > 0)
            {

                its++;
                k--;
            }
            its->flag = 0;
            break;
        }
        else
        {
            while(k < iter->getCount())
            {
                its++;
                k++;
            }
        }
    }
    //如果没有此m3u8文件,选中失败
    if(iter == m_m3u8_list.end())
        return BS_INVALID;
    return BS_SUCCESS;
}
state_t M3U8Clip::removeTS(const char* m3u8file, uint32_t start, uint32_t size)
{
    its = m_ts_flag.begin();
    for(iter = m_m3u8_list.begin(); iter != m_m3u8_list.end(); ++iter)
    {
        uint32_t k =0;
        if(strcmp(m3u8file,iter->getM3u8name()) == 0)
        {
            //如果i大于m3u8文件中ts的个数,则无法选中
            if(start >= iter->getCount())
                return BS_INVALID;
            //k=i;
            //its++;
            //保证开始位置小于总长度
            if(start >= iter->getCount())
                return BS_INVALID;
            //保证start+size小于总长度
            size = size > (iter->getCount()-start)?(iter->getCount()-start):size;

            while(start>0)
            {
                its++;
                start--;
            }
            while(size>0)
            {
                its->flag =0;
                size--;
                its++;
            }
            break;
        }
        else
        {
            while(k < iter->getCount())
            {
                its++;
                k++;
            }
        }
    }
    //如果没有此m3u8文件,选中失败
    if(iter == m_m3u8_list.end())
        return BS_INVALID;
    return BS_SUCCESS;
}

//文件拷贝
state_t M3U8Clip::CopyFile(char *SourceFile,const char *NewFile)
{
    ifstream in;
    ofstream out;
    in.open(SourceFile,ios::binary);//打开源文件
    if(in.fail())//打开源文件失败
    {
        cout<<"Error: Fail to open the source file " << SourceFile << endl;
        in.close();
        out.close();
        return BS_INVALID;
    }

    out.open(NewFile,ios::binary);//创建目标文件
    if(out.fail())//创建文件失败
    {
        cout<<"Error: Fail to create the new file." << NewFile << endl;
        out.close();
        in.close();
        return BS_INVALID;
    }

    out<<in.rdbuf();
    out.close();
    in.close();
    return BS_SUCCESS;
}
// 将选中的ts生成一个新的m3u8，ts需要重新拷贝出一份
state_t M3U8Clip::build(const char* path)
{
    ofstream in;
    string NewFile;
    string m3u8path;
    string name = getname();
    int count = 0;
    for(its = m_ts_flag.begin();its != m_ts_flag.end();++its)
    {
        if((int)its->flag == 1)
        {
            count++;
        }
    }
    char c[8]; 
    sprintf(c, "%d", --count);
    m3u8path.append(path).append("/").append(name).append("0-");
    m3u8path += c;
    m3u8path.append(".m3u8");
    string file = NewFile.append(path)+"/"+name;
    string tsname;
    in.open(m3u8path.c_str(), ios::trunc); //ios::trunc表示在打开文件前将文件清空,由于是写入,文件不存在则创建
    in<<"#EXTM3U"<<'\n';
    in<<"#EXT-X-VERSION:3"<<'\n';
    in<<"#EXT-X-TARGETDURATION:"<<targetDuration<<'\n';
    in<<"#EXT-X-MEDIA-SEQUENCE:0"<<'\n';
    count = 0;
    for(its = m_ts_flag.begin();its != m_ts_flag.end();++its)
    {
        if((int)its->flag == 1)
        {
            in << "#EXTINF:" << its->m3u8.getTS(its->index)->time << "," << "\n";
            tsname = name;
            sprintf(c, "%d", count);
            in<<(tsname+c).append(".ts") << "\n";
            NewFile = file;
            CopyFile(its->m3u8.getTS(its->index)->ts_url, (NewFile+c).append(".ts").c_str());
            count++;
        }
        //cout<<<<<<endl;
    }
    in<<"#EXT-X-ENDLIST";
    in.close();
    return BS_SUCCESS;
}

static int getMacAddress(char *addr)
{
#ifdef _WIN32

#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
    sprintf(addr, "ios_simulator");
#elif TARGET_OS_IPHONE
    sprintf(addr, "iphone");
#elif TARGET_OS_MAC
    FILE *fp = popen("ifconfig en0 | awk '/ether/{print $2}'", "r");
    fscanf(fp, "%s", addr);
    pclose(fp);
#else 
    return -1;
#endif

#elif __linux__
    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    snprintf(addr, 13, "%02X:%02X:%02X:%02X:%02X:%02X\0",
            (unsigned char) ifr.ifr_hwaddr.sa_data[0],
            (unsigned char) ifr.ifr_hwaddr.sa_data[1],
            (unsigned char) ifr.ifr_hwaddr.sa_data[2],
            (unsigned char) ifr.ifr_hwaddr.sa_data[3],
            (unsigned char) ifr.ifr_hwaddr.sa_data[4],
            (unsigned char) ifr.ifr_hwaddr.sa_data[5]);
#endif
    return 1;
}

//获取m3u8文件名
const char* M3U8Clip::getname()
{
    string m3u8file;
    //获取本机mac地址
    char mac_addr[16];
    int res = getMacAddress(mac_addr);
    if (res == -1) {
        sprintf(mac_addr, "unknown");
    }
    m3u8file.append(mac_addr).append("_");
    //获取系统时间
    char buf[28];
    struct timeval tv1;
    gettimeofday(&tv1, NULL);
    struct timeval tv;
    struct tm    tm;
    int  len = 28;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm);
    strftime(buf, len, "%Y%m%d_%H%M%S", &tm);
    len = strlen(buf);
    sprintf(buf+len, ".%d", ((int)(tv.tv_usec))/1000);

    m3u8file.append(buf).append("_hd.");

    //cout<<m3u8file<<endl;
    return m3u8file.c_str();
}
