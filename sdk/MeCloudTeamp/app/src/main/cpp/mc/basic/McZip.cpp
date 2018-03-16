/**
 * file :	McZip.cpp
 * author :	Rex
 * create :	2016-09-22 20:08
 * func : 
 * history:
 */

#include <string.h>
#include "bs.h"
#include "McZip.h"
#include "zlib.h"
#include "McFile.h"

using namespace mc;

int64_t Zip::compress(byte *data, uint32_t ndata, byte* zdata, uint32_t nzdata){
    z_stream c_stream;
    int err = 0;
    
    if(data && ndata > 0)
    {
        c_stream.zalloc = (alloc_func)0;
        c_stream.zfree = (free_func)0;
        c_stream.opaque = (voidpf)0;
        if(deflateInit(&c_stream, Z_DEFAULT_COMPRESSION) != Z_OK) return -1;
        c_stream.next_in  = data;
        c_stream.avail_in  = ndata;
        c_stream.next_out = zdata;
        c_stream.avail_out  = nzdata;
        while (c_stream.avail_in != 0 && c_stream.total_out < nzdata)
        {
            if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
        }
        if(c_stream.avail_in != 0) return c_stream.avail_in;
        for (;;) {
            if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
            if(err != Z_OK) return -1;
        }
        if(deflateEnd(&c_stream) != Z_OK) return -1;
        return c_stream.total_out;
    }
    return -1;
}

int64_t Zip::decompress(byte* zdata, uint32_t nzdata, byte* data, uint32_t ndata)
{
    int err = 0;
    z_stream d_stream; /* decompression stream */
    
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in  = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;
    if(inflateInit(&d_stream) != Z_OK) return -1;
    while (d_stream.total_out < ndata && d_stream.total_in < nzdata) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if(err != Z_OK){
            return -1;
        }
    }
    if(inflateEnd(&d_stream) != Z_OK) return -1;
    return d_stream.total_out;
}

Crypt::Crypt(){
    m_bytes = NULL;
}
Crypt::~Crypt(){
    if (m_bytes!=NULL) {
        free(m_bytes);
    }
}

int64_t Crypt::encrypt(byte* src, uint32_t src_size){
    // 头4个字节存储长度
    byte* bytes = (uint8_t*)malloc(src_size+4);
    memcpy(bytes, &src_size, 4);
    
    int64_t zip_size = Zip::compress(src, src_size, bytes+4, src_size);
    if (zip_size>0)
    {
        unsigned char bit = 0;
        for(long i=0; i<zip_size+4; i++){
            bit++;
            bytes[i] += (bit/10);
        }
        
        if (m_bytes==NULL) {
            m_bytes = (uint8_t*)calloc(1, (zip_size+4)*5);
        }
        else if(m_size < (zip_size+4)*5){
            free(m_bytes);
            m_bytes = (uint8_t*)calloc(1, (zip_size+4)*5);
        }
        for(int i=0; i<zip_size+4; i++){
            sprintf((char*)m_bytes+ strlen((const char*)m_bytes), "0x%x,", (unsigned char)bytes[i]);
        }
        
        free(bytes);
        return strlen((const char*)m_bytes);
    }

    free(bytes);
    return zip_size;
}

int64_t Crypt::decrypt(byte* src_buffer, uint32_t src_buffer_size){
    byte* src = (byte*)malloc(src_buffer_size);
    char* ptr = (char*)src_buffer;
    uint32_t src_size = 0;
    int code;
    while (sscanf(ptr, "0x%x,", &code)>0 && (uint32_t)(ptr-(char*)src_buffer)<src_buffer_size) {
        src[src_size] = code;
        src_size++;
        
        ptr = strstr(ptr, ",");
        ptr++;
    }
    
    unsigned char bit = 0;
    for(long i=0; i<src_size; i++){
        bit++;
        src[i] -= (bit/10);
    }
    
    uint32_t size;
    memcpy(&size, src, 4);
    // 头4个字节存储长度, 多申请一个字节为'\0'
    if (m_bytes==NULL) {
        m_bytes = (uint8_t*)malloc(size+1);
        m_size = size;
    }
    else if(m_size < size){
        free(m_bytes);
        m_bytes = (uint8_t*)malloc(size+1);
        m_size = size;
    }
    
    memset(m_bytes, size+1, 0);
    int64_t zip_size = Zip::decompress(src+4, src_size, m_bytes, size);
    if (zip_size > 0) {
        m_bytes[zip_size] = '\0';
    }
    
    return zip_size;
}

state_t Crypt::decryptFromFile(const char* path){
    mc::File file(path);
    if (!file.exist()) {
        return BS_NOTFOUND;
    }
    
    byte* b = file.read();
    if (b==NULL) {
        return BS_INVALID;
    }
    
    int64_t size = decrypt(b, (uint32_t)file.size());
    if (size!=(uint32_t)strlen((const char *)m_bytes) || size!=m_size) {
        err_log("decrypt[%ld] size[%ld] len[%u]", size, m_size, strlen((const char*)m_bytes));
        return BS_INVALID;
    }
    return (state_t)size;
}
state_t Crypt::encryptToFile(const char* path, byte* src, uint32_t src_size){
    int64_t size = encrypt(src, src_size);
    if (size<0) {
        return BS_INVALID;
    }
    
    return mc::FileManager::write(path, m_bytes, (uint32_t)size);
}
