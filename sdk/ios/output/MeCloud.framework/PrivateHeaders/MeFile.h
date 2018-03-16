/**
 * file :	MeFile.h
 * author :	Rex
 * create :	2016-09-20 23:46
 * func : 
 * history:
 */

#ifndef	__MEFILE_H_
#define	__MEFILE_H_

#include "bs.h"
#include "MeObject.h"
#include "McHttp.h"
#include "McZip.h"
#ifdef __IOS__
#include "MeIOSHttpFileCallBack.h"
#else
#include "MeHttpFileCallBack.h"
#endif

typedef enum ME_HTTPFILE_TYPE{
    ME_HTTPFILE_Default,        // 普通文件对象
    ME_HTTPFILE_DOWNLOAD,       // 上传文件对象
    ME_HTTPFILE_UPLOAD,         // 下载文件对象
}me_file_type;

using namespace mc;
class MeFile: public MeObject{
public:
    MeFile(me_file_type type = ME_HTTPFILE_DOWNLOAD, const char* className = "File", JSONObject* obj = NULL);
    MeFile(MeFile* file);
    MeFile(MeObject *meObject, bool auth = false);
    MeFile(const char* objectId, const char* className = "File");
    ~MeFile();
    
    void init();
    
    const char* getUrl();
    
    /// 返回处理文件类型（目前有上传和下载）
    me_file_type fileType();
    
    /// 返回存储路径
    const char* filePath();
    void setFilename(const char* filename);
    
    /// 解压
    bool unzip();
    
    /// 解压后文件名，如果不是zip文件返回NULL
//    const char* unzipFileName();
    /// 解压后文件路径，如果不是zip文件返回nil
    const char* unzipFilePath();
    
    /// 根目录
    static const char*  rootPath();
    
    // 图片文件
    const char* imageUrl(int width, int height);
  
    const char* imageCropUrl(int x, int y, int width, int height);
  
    const char* imageUrl();
    
protected:
    static const char*  m_root_path;
    
    char        m_name[1024];
    char        m_path[1024];
    char        m_image_url[URL_SIZE];
    
//    char        m_unzip_name[256];
//    char        m_unzip_path[1024];
    me_file_type me_type;
    const char *me_platform = "oss";
    const char *me_bucket = "";
    
private:
};

#endif
