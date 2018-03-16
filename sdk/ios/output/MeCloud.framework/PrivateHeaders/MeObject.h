/**
 * file :	MeObject.h
 * author :	bushaofeng
 * create :	2016-08-27 01:09
 * func : 
 * history:
 */

#ifndef	__MEOBJECT_H_
#define	__MEOBJECT_H_

#include <string>
#include <map>
#include <vector>
#include <iterator>
#include "MeCloud.h"
#include "JSON.h"
#include "MeACL.h"

#define ME_CLASS_SIZE   64
using namespace mc;
using namespace std;

#define ME_TYPE_POINTER  "__pointer"
class MeObject: public JSONObject{
public:
    MeObject(); // 只用于MeCallback生成MeObject数组，数组必须使用默认构造函数
    MeObject(const char* objectId, const char* className);
    // 默认转移释放权限，如果不转移会深拷贝
    MeObject(MeObject* obj, bool auth = true);
    MeObject(const char* className, JSONObject* obj = NULL);
    ~MeObject();

    const char* className();
    inline const char *objectId() { return m_objectid; }
    inline const char *url() { return m_url; }
    
    void setClassName(const char* className);
    void setDb(const char* db);
    // 添加唯一索引, 只在post时生效
    void addUniqueKey(const char* key);
    
    // 权限操作
    void setACL(MeACL* acl);
    MeACL getACL();

    virtual void put(const char* name, const char* value);
    virtual void put(const char* name, double value);
    virtual void put(const char* name, int value);
    virtual void put(const char* name, long value);
    virtual void put(const char* name, float value);
    virtual void put(const char* name, bool value);
    // 对象嵌套
    virtual void set(const char* name, MeObject* obj);
    virtual void add(const char* name, MeObject* object);
    virtual void set(const char* name, JSONArray* array, const char* childClassName);
    
    MeObject objectValue(const char* key);
    JSONArray arrayValue(const char* key);
    
	void increase(const char* key, int num);
    void increase(const char* key, long num);
	void increase(const char* key, float num);

    //谨慎使用，目前不提供该功能
    void deleteObject(MeCallback* callback);
    
	void save(MeCallback* callback);
#ifdef __IOS__
    void save(MeCallbackBlock block);
#endif

    // 是否转移释放权限，如果不转移则深拷贝
    virtual void copy(JSONObject* obj, bool auth= true);
    void copySelf(JSONObject *obj, bool auth = true);
    
    virtual void clear();
    /// 从文件反序列化
    virtual bool deserialize(const char* path);
    
protected:
    char                    m_classname[ME_CLASS_SIZE];
    char                    m_db[ME_CLASS_SIZE];
    char                    m_url[URL_SIZE];
    MeACL                   objectAcl;                   // 需要new出来的对象
    
    char                    m_objectid[BS_UNIQUE_ID_LENGTH];
    JSONObject              m_set_dirty;
    JSONObject              m_inc_dirty;
    
    //TODO: 暂时只考虑了从服务器下载数据的解析
    map<string, MeObject*>  m_object_map;
    map<string, JSONArray*> m_array_map;
    map<string, string>     m_child_classname;
    vector<string>          m_unique_key;
    
    // 存储save传进来的callback，目前只支持一个callback，一次save完成之后才能进行下次save
    MeCallback*             m_callback;
    
private:
    void init();
    
    void initPostUrl();
    void saveData(MeCallback* callback);
    void postData(MeCallback* callback);
    void putData(MeCallback* callback);
    
    // 嵌套类数据
    void saveNestedData(MeCallback* callback);
    void putNestedData(MeCallback* callback, JSONArray *array);
    void postNestedData(MeCallback* callback, JSONArray *array);
    void clearMap();
};

#endif
