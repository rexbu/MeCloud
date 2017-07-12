/**
 * file :	Database.h
 * author :	Rex
 * create :	2017-06-15 17:07
 * func : 
 * history:
 */

#ifndef	__DATABASE_H_
#define	__DATABASE_H_

#include "JSON.h"
#include "sqlite3.h"
#include <map>

using namespace mc;
using namespace std;
class Database{
public:
    Database(const char* name){
        m_db_name = name;
    }
    
    virtual void createTable(const char* table, map<string, string> columns) = 0;
    virtual void insert(const char* table, JSONObject* obj) = 0;
    virtual void query(const char* table, JSONObject* obj) = 0;
    virtual void remove(const char* table, JSONObject* obj) = 0;
    virtual bool update(const char* table, JSONObject* obj) = 0;
    virtual bool query_forward() = 0;
    virtual bool column_int() = 0;
    virtual bool column_string() = 0;
    virtual bool column_double() = 0;
    virtual bool column_bool() = 0;
    
protected:
    std::string     m_db_name;
};

class SQLiteDb: public Database{
public:
    SQLiteDb(const char* name);
    ~SQLiteDb();
    
    virtual void createTable(const char* table, JSONObject* columns);
    virtual void insert(const char* table, JSONObject* obj);
    virtual void query(const char* table, JSONObject* obj);
    virtual void remove(const char* table, JSONObject* obj);
    virtual bool update(const char* table, JSONObject* obj);
    virtual bool query_forward();
    virtual bool column_int();
    virtual bool column_string();
    virtual bool column_double();
    virtual bool column_bool();
    
protected:
    std::string     m_db_path;
    sqlite3*        m_db;
    sqlite3_stmt*   m_stmt;
};
#endif
