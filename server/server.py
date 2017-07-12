# coding=utf-8
import tornado.ioloop
import tornado.web
import tornado.httpserver
import pymongo
import sys
import os
from ConfigParser import *
from api.BaseHandler import *
from api.ClassHandler import *
from api.UserHandler import *
from api.RelationHandler import *
from helper.DbHelper import *
from lib import *
import sys

class Application(tornado.web.Application):
    def __init__(self):
        # 读取配置文件
        self.config = ConfigParser()
        self.config.read('./config')
        self.version = self.config.get('global', 'version')
        self.project = self.config.get('global', 'project')
        log.info("version:%s project:%s", self.version, self.project)

        urls = self.config.options('handlers')
        handlers = []
        for url in urls:
            handle = (url, self.config.get('handlers', url))
            handlers.append(handle)
        
        try:
            self.wx_redirect = self.config.get('global', 'WX_REDIRECT')
            self.wx_appid = self.config.get('global', 'WX_APPID')
            self.wx_appsecret = self.config.get('global', 'WX_APPSECRECT')
        except Exception, e:
            pass
        
        settings = dict(
            cookie_secret="bZJc2sWbQLKos6GkHn/VB9oXwQt8S0R0kRvJ5/xJ89E=",
            #"xsrf_cookies": True,
            debug=True,
            template_path=os.path.join(os.path.dirname(__file__), "views"),
            static_path=os.path.join(os.path.dirname(__file__), "static"),
            autoreload = True,
        )

        tornado.web.Application.__init__(self, handlers, **settings)

if __name__ == "__main__":
    #设置系统默认编码为utf8
    reload(sys)
    sys.setdefaultencoding('utf-8')
    config = ConfigParser()
    config.read('./config')
    mode = config.get('global', 'mode')
    MeFile.bucketName = config.get(mode, 'BUCKET')
    MeFile.domain = config.get(mode, 'DOMAIN')
    # 定义全局数据库
    db = config.get('global', 'db')
    if db:
        Db.name = db

    log.info("########## Start:%s ############"%mode)

    # 是否是accessstoken server
    global access_token
    global jsapi_ticket
    try:
        access_server = config.get('global', 'WX_ACCESSTOKEN_SERVER')
        if access_server:
            access_token = wx.accessTokenFromWx()
            jsapi_ticket = wx.jsapiTicketFromWx()
    except Exception, e:
        access_server = False
        print  "Not Weixin Config"

    # mongodb及oss配置
    if mode=='online':
        Db.conn = pymongo.MongoClient([config.get(mode, 'MONGO_ADDR1'), config.get(mode, 'MONGO_ADDR2')], replicaSet=config.get(mode, 'REPLICAT_SET'))
        Db.conn.admin.authenticate(config.get(mode, 'USERNAME'), config.get(mode, 'PASSWORD'))
        MeFile.bucketUrl = 'http://'+MeFile.domain
        MeFile.bucket = oss2.Bucket(MeFile.auth, 'http://'+MeFile.domain, MeFile.bucketName, is_cname=True)
    else:
        Db.conn = pymongo.MongoClient('127.0.0.1', 27017)
        MeFile.bucketUrl = 'http://'+ MeFile.bucketName+'.'+MeFile.domain
        MeFile.bucket = oss2.Bucket(MeFile.auth, 'http://'+MeFile.domain, MeFile.bucketName)
    # 单工程模式
    if config.get('global', 'project_type')==0:
        Db.name = config.get('global', 'db')

    print MongoDb.conn
    print "Listen Port: "+config.get(mode, 'PORT')

    # xheaders获取真实ip
    server = tornado.httpserver.HTTPServer(Application(), xheaders=True)
    server.listen(config.get(mode, 'PORT'))
    tornado.ioloop.IOLoop.instance().start()
