/**
 * file :	McZip.h
 * author :	bushaofeng
 * create :	2016-08-25 17:17
 * func :   zip解压缩
 * history:
 */

#ifndef	__MCZIP_H_
#define	__MCZIP_H_

#include "bs.h"
/*
#include "Common.h"

extern bool unzip(const char* path, const char* destination);
*/

namespace mc {
/*
class ZipCallback{
public:
	virtual void progress(const char *entry, unz_file_info zipInfo, long entryNumber, long total) = 0;
	virtual void complete(const char *path, bool succeeded, const char* error) = 0;
};

class ZipArchiveCallback{
public:
	virtual void willUnzip(const char* path, unz_global_info zipinfo) = 0;
	virtual void didUnzip(const char* path, unz_global_info zipinfo, const char* unzip_path) = 0;
	virtual bool shouldUnzip(uint32_t index, uint32_t total, const char* archivePath, unz_file_info fileinfo) = 0;
	virtual void willUnzip(uint32_t index, uint32_t total, const char* archivePath, unz_file_info fileinfo) = 0;
	virtual void didUnzip(uint32_t index, uint32_t total, const char* archivePath, unz_file_info fileinfo) = 0;
	virtual void didUnzip(uint32_t index, uint32_t total, const char* archivePath, const char* unzip_filepath) = 0;

	virtual void progress(uint64_t loaded, uint64_t total) = 0;
	virtual void didUnzipFile(const char* zipfile, const char* entrypath, const char* destpath) = 0;
};
*/
class Zip{
public:
    // 压缩与解压字符串
    static int64_t compress(byte *data, uint32_t ndata, byte* zdata, uint32_t nzdata);
    static int64_t decompress(byte* zdata, uint32_t nzdata, byte* data, uint32_t ndata);
	
};

class Crypt{
public:
    Crypt();
    ~Crypt();
    int64_t encrypt(byte* src, uint32_t src_size);
    int64_t decrypt(byte* src, uint32_t src_size);
    
    state_t decryptFromFile(const char* path);
    state_t encryptToFile(const char* path, byte* src, uint32_t src_size);
    byte* bytes(){ return m_bytes; }
protected:
    uint8_t*    m_bytes;
    uint32_t    m_size;
};

}

#endif
