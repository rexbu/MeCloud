/**
 * file :	utp.cpp
 * author :	Rex
 * create :	2017-03-23 13:31
 * func : 
 * history:
 */

#include "bs.h"
#include "utp.h"
#include "SocketFrame.h"

#pragma --mark "发送线程"
class UTPThread: public LoopThread{
public:
    UTPThread(UTP* utp){
        m_utp = utp;
        m_timeout.tv_sec = 0;
        m_timeout.tv_usec = 5000;
    }
    
    virtual void loop(){
        int rv = select(0, NULL, NULL, NULL, &m_timeout);
        if (m_utp!=NULL) {
            m_utp->sendSerial();
        }
    }
    
protected:
    UTP*            m_utp;
    struct timeval  m_timeout;
};

#pragma --mark "UTP定义"
UTP* UTP::m_instance = NULL;

void UTP::initialize(int port){
    if (m_instance!=NULL) {
        delete m_instance;
    }
    m_instance = new UTP(port);
}

UTP* UTP::shareInstance(){
    if (m_instance==NULL) {
        m_instance = new UTP(12080);
    }
    return m_instance;
}

UTP::UTP(int port){
    m_sock = socket_udp(BS_TRUE);
    state_t st = bs_sock_bind(m_sock, port);
    if (st!=BS_SUCCESS) {
        err_log("utp bind[%d] error: %d", port, st);
    }
    
    m_identifier = 0;
    m_send_id = 0;
    m_recv_queue.resize(UTP_BUFFER_SIZE);
    m_send_queue.resize(UTP_BUFFER_SIZE);
    for (int i=0; i<UTP_BUFFER_SIZE; i++) {
        m_recv_queue[i].statflag = 0;
        m_send_queue[i].statflag = 0;
    }
    SocketFrame::instance()->append(this);
    
    m_send_thread = new UTPThread(this);
    m_send_thread->start();
}

UTP::~UTP(){
    if (m_send_thread!=NULL) {
        m_send_thread->stop();
        delete m_send_thread;
    }
}

void UTP::onRead(){
    utp_slice_t     slice;
    sockaddr_in     addr;
    socklen_t       sin_size = sizeof(struct sockaddr_in);
    
    int len = (int)recvfrom(m_sock, &slice, sizeof(utp_slice_t), 0, (struct sockaddr*)&addr, &sin_size);
    if (len<0) {
        onError(errno);
        return;
    }
    
    utp_pack_t* pack = &m_recv_queue[slice.identifier>>6];
    if(pack->slices.size()==0){
        pack->slices.resize(slice.slicesize);
    }
    pack->slices[slice.serial] = slice;
    pack->statflag |= 1<<slice.serial;
    // 检查当前帧
    if (pack->statflag == ~(0xffffffff<<(slice.slicesize))) {
        data_t* data = bs_new(data);
        for (int i=0; i<slice.slicesize; i++) {
            data_append(data, (char*)pack->slices[i].data, pack->slices[i].datasize);
        }
        // 处理
        pack->statflag = 0;
        pack->slices.clear();
    }
}
void UTP::onWrite(){
    
}
void UTP::onError(int error){
    
}
void UTP::onMessage(sock_msg_t* msg){
    
}

void UTP::send(const char* ip, int port, const char* buffer, uint32_t size){
    sockaddr_in     addr;
    bs_sock_addr(&addr, ip, port);
    send(&addr, buffer, size);
}

void UTP::send(sockaddr_in* addr, const char* buffer, uint32_t size){
    if (buffer == NULL || size<=0) {
        return;
    }
    
    int slice_num = ceil(size*1.0/UTP_PAYLOAD_SIZE);
    
    utp_pack_t* pack = &m_send_queue[m_identifier%UTP_BUFFER_SIZE];
    pack->slices.resize(slice_num);
    pack->statflag = ~(0xffffffff<<slice_num);
    memcpy(&pack->addr, addr, sizeof(sockaddr_in));
    
    for (int i=0; i<slice_num; i++) {
        utp_slice_t* slice = &pack->slices[i];
        slice->serial = i;
        slice->slicesize = slice_num;
        slice->identifier = m_identifier;
        int left = size - i*UTP_PAYLOAD_SIZE;
        // 如果剩余比UTP_PAYLOAD_SIZE大，说明还没有到最后一个slice
        slice->datasize = left>UTP_PAYLOAD_SIZE ? UTP_PAYLOAD_SIZE:left;
        memcpy(slice->data, buffer+i*UTP_PAYLOAD_SIZE, slice->datasize);
    }
    
    m_identifier++;
}

void UTP::sendSerial(){
    utp_pack_t* pack = &m_send_queue[m_send_id%UTP_BUFFER_SIZE];
    if (pack->statflag==0) {
        return;
    }
    
    // flag表示已经发送的slice，例如00000111,表示已经发送了前三slice，flag+1取2的对数计算即为要发送的下一slice
    int flag = pack->statflag ^ (~(0xffffffff<<pack->slices[0].slicesize));
    int slice_id = bs_log2(flag+1);
    utp_slice_t* slice = &pack->slices[slice_id];
    int rv = (int)sendto(m_sock, slice, UTP_SLICE_SIZE(slice), 0, (struct sockaddr*)&pack->addr, (socklen_t)sizeof(struct sockaddr_in));
    if (rv<0) {
        err_log("utp send error: %d/%d", rv, errno);
        return;
    }
    err_log("utp send: %d/%d %d", slice->serial, slice->identifier, UTP_SLICE_SIZE(slice));
    // 标记当前slice已经发送
    pack->statflag &= ~(1<<slice_id);
    if (pack->statflag==0) {
        m_send_id++;
    }
}
