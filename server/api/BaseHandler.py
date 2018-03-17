# -*- coding: utf-8 -*-
import types

import tornado.web
from mecloud.lib import *
from mecloud.model.MeError import *
from mecloud.model.MeACL import *
from mecloud.model.DevelopUser import *
from mecloud.helper.AppHelper import *
from mecloud.helper.RedisHelper import RedisDb
from datetime import *
import json, time, functools


class BaseConfig:
    mode = "online"
    deleteClass = []
    accessNoClass = []
    projectClass = {}
    pushUrl = None
    adminUser = {}
    wsserver = None

class BaseHandler(tornado.web.RequestHandler):
    needCrypto = True

    # aclList = ["read", "create", "write", "delete"]

    def __init__(self, *args, **kwargs):
        tornado.web.RequestHandler.__init__(self, *args, **kwargs)
        self.project = self.application.project

    def get_current_user(self):
        userid = self.get_secure_cookie("u")
        if userid is None:
            userid = self.get_userid_from_cookie(str(self.get_cookie("u")))
        return userid

    def prepare(self):
        log.info("url------------:%s", self.request.path)
        method = self.request.method
        userid = self.get_current_user()
        log.info("method---------:%s", method)
        print 'header:', self.request.headers
        if BaseConfig.mode == "online":
            self.needCrypto = True
        else:
            self.needCrypto = False

        try:
            # print 'X-MeCloud-Debug:', self.request.headers["X-MeCloud-Debug"]
            if int(self.request.headers["X-MeCloud-Debug"]) == 1:
                self.needCrypto = False
        except Exception, e:
            pass
        print 'userid:', userid
        if userid:
            if userid in BaseConfig.adminUser:
                self.needCrypto = False
            userQuery = MeQuery("User")
            self.user = userQuery.get(userid)
            # 如果数据库中找不到用户，则清空cookie
            if not self.user and self.request.uri != '/wx/logout':
                # self.render("error.html", user=None, wxconfig=self.wxconfig)
                print 'user is missing, userid:', userid
                self.write(ERR_USER_MISSING.message)
                self.finish()
            # 延续redis中cookie时间
            try:
                cookie_in_header = self.request.headers['Cookie'].split('"')[1]
                if cookie_in_header:
                    self.set_user_cookie_record(cookie_in_header, userid)
            except Exception, e:
                print 'set cookie to redis has an error:', e

        # 检查包体
        if method.upper() in ['POST', 'PUT']:
            try:
                if self.needCrypto:
                    self.request.body = crypto.decrypt(self.request.body)
                if self.request.body:
                    self.jsonBody = json.loads(self.request.body)
            except Exception, e:
                print e
                self.write(ERR_INVALID.message)
                self.finish()
                return
            # if method.upper() in ['POST']:
            #     self.check_field()
            print 'self.needCrypto:', self.needCrypto
            try:
                if self.needCrypto and self.request.arguments:
                    arguments = {}
                    for paramStr in self.request.arguments:
                        pass
                    # for key in self.request.arguments:
                    #     arguments[crypto.decrypt(key)] = [crypto.decrypt(self.get_argument(key))]
                    params = crypto.decrypt(paramStr).split("&")
                    for param in params:
                        keys = param.split("=")
                        arguments[keys[0]] = [keys[1]]
                    self.request.arguments = arguments

            except Exception, e:
                self.write(ERR_INVALID.message)
                self.finish()
                return
        else:
            try:
                if self.needCrypto and self.request.arguments:
                    arguments = {}
                    for paramStr in self.request.arguments:
                        pass
                    # for key in self.request.arguments:
                    #     arguments[crypto.decrypt(key)] = [crypto.decrypt(self.get_argument(key))]
                    params = crypto.decrypt(paramStr).split("&")
                    for param in params:
                        keys = param.split("=")
                        arguments[keys[0]] = [keys[1]]
                    self.request.arguments = arguments
            except Exception, e:
                self.write(ERR_INVALID.message)
                self.finish()
                return

    def wxConfig(self):
        wxconfig = {}
        wxconfig['appId'] = config.wx['appId']
        wxconfig['timestamp'] = int(time.time())
        wxconfig['nonceStr'] = 'Wm3WZYTPz0wzCcnW'
        wxconfig['url'] = self.request.full_url()
        wxconfig['signature'] = wx.jsapiSignature(wxconfig['nonceStr'], wxconfig['timestamp'], wxconfig['url'])
        return wxconfig

    def connection(self):
        return self.application.conn;

    def setUserCookie(self, userid):
        self.set_secure_cookie("u", userid)
        new_cookie = str(self._new_cookie.get('u')).split('=')[1].split(';')[0].replace('"', '')
        self.set_user_cookie_record(new_cookie, userid)
        USER_COOKIE_TIME_OUT = 3600 * 24 * 30
        RedisDb.setex(new_cookie, userid, USER_COOKIE_TIME_OUT)

    def set_default_headers(self):
        self.set_header("Access-Control-Allow-Origin", "*")
        self.set_header("Access-Control-Allow-Headers", "x-requested-with")
        self.set_header('Access-Control-Allow-Methods', 'POST, GET, OPTIONS')

    def set_user_cookie_record(self, cookie, userid):
        USER_COOKIE_TIME_OUT = 3600 * 24 * 30
        RedisDb.setex(userid, cookie, USER_COOKIE_TIME_OUT)
        return RedisDb.setex(cookie, userid, USER_COOKIE_TIME_OUT)

    def get_userid_from_cookie(self, cookie):
        return RedisDb.get(cookie)

    def write(self, msg):
        if type(msg) is types.DictType:
            self.filter_field(msg)
            msg = json.dumps(msg, cls=MeEncoder)
        if self.needCrypto:
            tornado.web.RequestHandler.write(self, crypto.encrypt(msg))
        else:
            tornado.web.RequestHandler.write(self, msg)

    # def write_error(self, status_code, **kwargs):
    #     if status_code == 403:
    #         self.write(ERR_CLASS_PERMISSION.message)
    #     else:
    #         self.write(ERR_INVALID.message)

    def get_login_url(self):
        self.write(ERR_LOGIN_AUTH_PERMISSION.message)
        self.finish()
        return

    def check_field(self, classname, obj):  # 检查字段存在与否，字段范围，字段默认值，字段关联，记录权限等
        try:
            checkInfo = BaseConfig.projectClass[classname]
        except Exception, ex:
            log.err("No classname in path, warn:%s", str(ex))
            self.write(ERR_PATH_PERMISSION.message)
            # self.finish()
            return False

        for item in obj:
            if item not in checkInfo['fields']:  # 字段存在与否
                log.err("Item %s not in classname %s", item, classname)
                self.write(ERR_INVALID.message)
                # self.finish()
                return False
            else:  # 字段存在，检查类型是否正确
                if not isinstance(obj[item], eval(checkInfo['fields'][item])) and checkInfo['fieldType']:
                    log.err("Item %s in classname %s type error", item, classname)
                    self.write(ERR_INVALID.message)
                    # self.finish()
                    return False
            if item in checkInfo['scope'] and checkInfo['fieldScope']:  # 字段范围
                if obj[item] not in checkInfo['scope'][item]:
                    log.err("Item %s in classname %s scope error", item, classname)
                    self.write(ERR_INVALID.message)
                    # self.finish()
                    return False
            if item in checkInfo['foreign'] and checkInfo['fieldForeign']:  # 外键映射
                clQuery = MeQuery(checkInfo['foreign'][item]['class'])
                if len(obj[item]) != 24 or not clQuery.get(obj[item]):
                    log.err("Item %s:%s in classname %s foreign error", item, obj[item], classname)
                    self.write(ERR_INVALID.message)
                    # self.finish()
                    return False
            if item in checkInfo['length'] and checkInfo['fieldLength']:  # 长度限制
                if len(obj[item]) != checkInfo['length'][item]:
                    log.err("Item %s in classname %s length error", item, classname)
                    self.write(ERR_INVALID.message)
                    # self.finish()
                    return False
        if "acl" not in obj:
            classAcl = checkInfo['classAcl']
            acl = {}
            currentUser = {}
            allUser = {}
            for key, value in classAcl.items():
                if value == "*":
                    allUser[key] = True
                elif value == "self":
                    currentUser[key] = True
                # TODO
                elif isinstance(value, list):
                    for val in value:
                        acl[obj[val]] = {key: True}
            if currentUser:
                acl[self.get_current_user()] = currentUser
            if allUser:
                acl['*'] = allUser

            obj['acl'] = acl
        return obj

    def filter_field(self, obj):
        if self.get_current_user() not in BaseConfig.adminUser:
            for field in ['acl', 'createAt', '_sid']:
                obj.pop(field, None)
        return obj

    ## 验证是否需要登录,需要返回True
    def verify_cookie(self, classname):
        try:
            verfyInfo = BaseConfig.projectClass[classname]
        except Exception, ex:
            log.err("No classname in path, warn:%s", str(ex))
            self.write(ERR_PATH_PERMISSION.message)
            return False
        if verfyInfo['cookie']:
            if self.get_current_user():
                return True
            else:
                return False
        else:
            return True


### 微信装饰器, state参数带跳转url
def wxauthenticated(method):
    @functools.wraps(method)
    def wrapper(self, *args, **kwargs):
        if not self.current_user:
            if self.request.method in ('GET', 'HEAD'):
                wx_login = 'https://open.weixin.qq.com/connect/oauth2/authorize?appid=%s&redirect_uri=%s&response_type=code&scope=snsapi_base&state=%s#wechat_redirect' % (
                    self.application.wx_appid, self.application.wx_redirect, quote(self.request.uri))
                self.redirect(wx_login)
                return
            raise tornado.HTTPError(403)
        return method(self, *args, **kwargs)

    return wrapper


class DevelopBaseHandler(BaseHandler):
    def prepare(self):
        self.userId = self.get_current_user()
        if self.userId != None:
            self.user = DevelopUser()
            if not self.user.get(self.userId):
                log.err('user not found: %s', self.userId)
                self.write(ERR_NOTFOUND.message)
                self.finish()
            # 读取message信息
            messageHelper = ClassHelper('develop', 'Message')
            self.user.setOverLoad('messageCount', messageHelper.query_count({'user:': self.userId}))
