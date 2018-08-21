#!/usr/bin/python
# coding=utf-8
import json
import os
from ConfigParser import ConfigParser

import oss2
import pymongo
import tornado.ioloop
import tornado.web
import tornado.httpserver

from mecloud.helper.ClassHelper import ClassHelper
from mecloud.helper.DbHelper import Db, MongoDb
from mecloud.helper.RedisHelper import *
from mecloud.lib import log, wx
from mecloud.model.MeFile import MeFileConfig
from mecloud.api.BaseHandler import BaseConfig
import sys, getopt
from mecloud.model.SmsCode import SmsCodeConfig

# 设置系统默认编码为utf8
reload(sys)
sys.setdefaultencoding('utf-8')


class Application(tornado.web.Application):
    def __init__(self, config_path=None, env='online'):
        # 读取配置文件
        self.port = None
        self.config = ConfigParser()
        configFile = ''
        if not config_path:
            opts, args = getopt.getopt(sys.argv[1:], "c:p:")
            if opts:
                for opt, arg in opts:
                    if opt == '-c':
                        configFile = arg
                    if opt == '-p':
                        self.port = arg
            else:
                print 'mecloud.py -c <configfile>'
                sys.exit()
        else:
            configFile = config_path
        if not configFile:
            print 'mecloud.py -c <configfile>'
            sys.exit()

        self.config.read(configFile)
        self.version = self.config.get('global', 'version')
        self.project = self.config.get('global', 'project')
        if not self.port:
            self.port = self.config.get('global', 'PORT')

        self.initOSS()
        self.initSMS()
        self.initRedis()
        self.initDb()

        if env=='offline':
            return
        log.info("version:%s project:%s", self.version, self.project)
        self.initWx()

        # handler 路径
        self.handlers = self.initHandlers()
        urls = self.config.options('handlers')
        for url in urls:
            handle = (url, self.config.get('handlers', url))
            self.handlers.append(handle)

        # 模板路径
        if self.config.has_option('global', 'TEMPLATE_PATH'):
            template_path = self.config.get('global', 'TEMPLATE_PATH')
        else:
            template_path = os.path.join(os.getcwd(), "views")
        # 静态文件路径
        if self.config.has_option('global', 'STATIC_PATH'):
            static_path = self.config.get('global', 'STATIC_PATH')
        else:
            static_path = os.path.join(os.getcwd(), "static")

        if self.config.has_option('global', 'mode'):
            self.mode = self.config.get('global', 'mode')
        else:
            self.mode = "develop"

        if self.config.has_option('global', 'DELETE_CLASS'):
            BaseConfig.deleteClass = self.config.get('global', 'DELETE_CLASS')

        if self.config.has_option('global', 'WSSERVER'):
            BaseConfig.wsserver = self.config.get('global', 'WSSERVER')

        settings = dict(
            cookie_secret="kr9ci0i0z$hti7YBnG7=gY6xvP&2ishfCqAAbW!sO3h0Opsd",
            # "xsrf_cookies": True,
            debug=True,
            template_path=template_path,
            static_path=static_path,
            autoreload=True,
        )
        log.info("template [%s] static [%s]", settings['template_path'], settings['static_path'])

        tornado.web.Application.__init__(self, self.handlers, **settings)

        if Db.conn:
            # arthur
            projectClassHelper = ClassHelper("ProjectClass")
            items = projectClassHelper.find({})
            for item in items:
                BaseConfig.projectClass[item['classname']] = item

            adminClassHelper = ClassHelper("Role")
            items = adminClassHelper.find({"role": "admin"})
            for item in items:
                BaseConfig.adminUser[item['user']] = item["role"]

            if self.config.has_option('global', 'ACCESS_NO_CLASS'):  # 对外不可通过class接口读取的Class
                BaseConfig.accessNoClass = eval(self.config.get('global', 'ACCESS_NO_CLASS'))

            if self.config.has_option('global', 'PUSHURL'):  # 对外不可通过class接口读取的Class
                BaseConfig.pushUrl = self.config.get('global', 'PUSHURL')
                log.info("Push url:%s", BaseConfig.pushUrl)

            self.initMongodbIndex()

    def start(self):
        server = tornado.httpserver.HTTPServer(self, xheaders=True)
        server.listen(self.port)
        log.info("server start on port:%s", self.port)
        tornado.ioloop.IOLoop.instance().start()

    def initHandlers(self):
        return [('/1.0/class/(\\w+)/(\\w+)', 'mecloud.api.ClassHandler.ClassHandler'),
                         ('/1.0/class/(\\w+)', 'mecloud.api.ClassHandler.ClassHandler'),
                         ('/1.0/query/(\\w+)', 'mecloud.api.QueryCountHandler.QueryCountHandler'),
                         ('/1.0/query/', 'mecloud.api.QueryCountHandler.QueryCountHandler'),
                         ('/1.0/user/(\\w+)', 'mecloud.api.UserHandler.UserHandler'),
                         ('/1.0/user/', 'mecloud.api.UserHandler.UserHandler'),
                         ('/1.0/file/', 'mecloud.api.FileHandler.FileHandler'),
                         ('/1.0/file/(\\w+)', 'mecloud.api.FileHandler.FileHandler'),
                         ('/1.0/upload/(\\w+)', 'mecloud.api.CmsFileUploadHandler.CmsFileUploadHandler'),
                         ('/1.0/file/download/(\\w+)', 'mecloud.api.FileDownloadHandler.FileDownloadHandler'),
                         ('/sms/(.+)', 'mecloud.api.SMSHandler.SMSHandler'),
                         ('/captcha/(.+)', 'mecloud.api.CaptchaHandler.CaptchaHandler'),
                         ('/wx/(\\w+)', 'mecloud.api.WxHandler.WxHandler'),
                         ('/1.0/follow/(\\w+)/(\\w+)/(\\d+)', 'mecloud.api.FollowerHandler.FollowerHandler'),
                         ('/1.0/follow/(\\w+)', 'mecloud.api.FollowerHandler.FollowerHandler'),
                         ('/statcount/query', 'mecloud.api.StatCountHandler.StatCountHandler'),
                         ('/1.0/black/(\\w+)/(\\w+)/(\\d+)', 'mecloud.api.BlacklistHandler.BlacklistHandler'),
                         ('/1.0/thirdpay/(\w+)', 'mecloud.api.PayHandler.PayHandler'),
                         ('/1.0/pay/(\w+)', 'mecloud.api.PayHandler.PayHandler'),
                         ('/1.0/wxpaycallback', 'mecloud.api.WxCallbackHandler.WxCallbackHandler'),
                         ('/1.0/alipaycallback', 'mecloud.api.AlipayCallbackHandler.AlipayCallbackHandler'),
                         ('/1.0/manager/(\w+)', 'mecloud.api.CmsInviteCodeHandler.CmsInviteCodeHandler')
                         ]

    def initOSS(self):
        if self.config.has_section('oss'):
            # oss相关初始化
            MeFileConfig.access_key_id = self.config.get('oss', 'OSS_ACCESS_KEY_ID')
            MeFileConfig.access_key_secret = self.config.get('oss', 'OSS_ACCESS_KEY_SECRET')
            MeFileConfig.bucket_name = self.config.get('oss', 'OSS_BUCKET_NAME')
            MeFileConfig.platform = self.config.get('oss', 'PLATFORM')
            MeFileConfig.endpoint = self.config.get('oss', 'OSS_ENDPOINT')
            MeFileConfig.sts_role_arn = self.config.get('oss', 'OSS_STS_ROLE_ARN')
            MeFileConfig.role_session_name = self.config.get('oss', 'OSS_ROLE_SESSION_NAME')
            MeFileConfig.region_id = self.config.get('oss', 'OSS_REGION_ID')
            MeFileConfig.auth = oss2.Auth(MeFileConfig.access_key_id, MeFileConfig.access_key_secret)
            MeFileConfig.bucketUrl = 'http://' + MeFileConfig.bucket_name + '.' + MeFileConfig.endpoint
            MeFileConfig.bucket = oss2.Bucket(MeFileConfig.auth, "http://" + MeFileConfig.endpoint,
                                              MeFileConfig.bucket_name)

    def initSMS(self):
        if self.config.has_section('oss'):
            # sms相关
            SmsCodeConfig.region = self.config.get('sms', 'SMS_REGION')
            SmsCodeConfig.access_key_id = self.config.get('sms', 'SMS_ACCESS_KEY_ID')
            SmsCodeConfig.access_key_secret = self.config.get('sms', 'SMS_ACCESS_KEY_SECRET')
            SmsCodeConfig.template_code = self.config.get('sms', 'SMS_TEMPLATE_CODE')
            SmsCodeConfig.sign_name = self.config.get('sms', 'SMS_SIGN_NAME')

    def initRedis(self):
        # set redis config and create redis pool
        if self.config.has_section('redis'):
            host = self.config.get('redis', 'HOST')
            pwd = self.config.get('redis', 'PASSWORD')
            port = self.config.get('redis', 'PORT')
            RedisDb.init(host, pwd, int(port))
            log.info('redis[%s:%s:%s] init success', host, port, pwd)

    def initDb(self):
        # 定义全局数据库
        db = self.config.get('global', 'db')
        if db:
            Db.name = db
        # mongodb及oss配置
        # mongodb及oss配置
        if self.config.has_option('mongodb', 'ADDR'):
            addr = self.config.get('mongodb', 'ADDR')
            MongoDb.connect(addr=addr)
            log.info('mongodb[%s] init success', addr)

    def initWx(self):
        ##>> 微信相关
        try:
            self.wx_redirect = self.config.get('wx', 'WX_REDIRECT')
            self.wx_appid = self.config.get('wx', 'WX_APPID')
            self.wx_appsecret = self.config.get('wx', 'WX_APPSECRECT')
        except Exception, e:
            pass
        # 是否是accessstoken server
        global access_token
        global jsapi_ticket
        try:
            access_server = self.config.get('wx', 'WX_ACCESSTOKEN_SERVER')
            if access_server:
                access_token = wx.accessTokenFromWx()
                jsapi_ticket = wx.jsapiTicketFromWx()
        except Exception, e:
            access_server = False
            log.err("Not Weixin Config")

    def initMongodbIndex(self):
        db = MongoDb()
        haveIndex = {}
        for classname in BaseConfig.projectClass:
            try:
                haveIndex[classname] = db.listIndex(classname)
                if {"_sid": 1} not in haveIndex[classname]:
                    db.index(classname, [("_sid", 1)])
                    log.info("classname:%s, index:%s", classname, json.dumps([("_sid", 1)]))
            except Exception, e:
                log.err("Error:%s, error:%s", classname, str(e))

        if self.config.has_option('mongodb', 'INDEX'):
            objs = self.config.get('mongodb', 'INDEX')
            objs = json.loads(objs)
            for obj in objs:
                try:
                    unique = obj.get("unique", False)
                    index = obj.get("index")
                    items = []
                    if index in haveIndex.get(obj["className"],[]):
                        continue
                    for key, value in index.items():
                        if value == 1:
                            items.append((key,pymongo.ASCENDING))
                        elif value == -1:
                            items.append((key, pymongo.DESCENDING))
                        else:
                            items.append((key, value))
                    db.index(obj["className"], items, unique=unique)
                    log.info("classname:%s, index:%s", obj["className"], json.dumps(items))
                except Exception, e:
                    log.err("JSON Error:%s, error:%s", obj["className"], str(e))
