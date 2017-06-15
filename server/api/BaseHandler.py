#-*- coding: utf-8 -*- 
import tornado.web
from lib import *
from model.MeError import *
from model.MeObject import *
from model.MeQuery import *
from model.MeACL import *
from model.MeUser import *
from model.MeRelation import *
from model.MeRole import *
from model.MeFile import *
from model.DevelopUser import *
from helper.AppHelper import *
from helper.CaptchaHelper import *
from bson import json_util
from datetime import *
from collections import namedtuple
import json,hashlib,os,urllib,time, functools

class BaseHandler(tornado.web.RequestHandler):
    def __init__(self, *args, **kwargs):
        tornado.web.RequestHandler.__init__(self, *args, **kwargs)
        self.project = self.application.project

    def get_current_user(self):
        return self.get_secure_cookie("u")

    def prepare(self):
        # weixin
        self.wxconfig = self.wxConfig()
        userid = self.get_current_user()
        if userid:
            userQuery = MeQuery("User")
            self.user = userQuery.get(userid)
            # 如果数据库中找不到用户，则清空cookie
            if not self.user and self.request.uri!='/wx/logout':
                self.render("error.html", user=None, wxconfig= self.wxconfig)

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
    
### 微信装饰器, state参数带跳转url
def wxauthenticated(method):
    @functools.wraps(method)
    def wrapper(self, *args, **kwargs):
        if not self.current_user:
            if self.request.method in ('GET', 'HEAD'):
                wx_login = 'https://open.weixin.qq.com/connect/oauth2/authorize?appid=%s&redirect_uri=%s&response_type=code&scope=snsapi_base&state=%s#wechat_redirect' % (self.application.wx_appid, self.application.wx_redirect, quote(self.request.uri))
                self.redirect(wx_login)
                return
            raise HTTPError(403)
        return method(self, *args, **kwargs)
    return wrapper


class DevelopBaseHandler(BaseHandler):
    def prepare(self):
        self.userId = self.get_current_user()
        if self.userId!=None:
            self.user = DevelopUser()
            if not self.user.get(self.userId):
                log.err('user not found: %s', self.userId)
                self.write(ERR_NOTFOUND.message);
                self.finish();
            # 读取message信息
            messageHelper = ClassHelper('develop', 'Message')
            self.user.setOverLoad('messageCount', messageHelper.query_count({'user:': self.userId}))
