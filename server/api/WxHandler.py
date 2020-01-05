#-*- coding: utf-8 -*-
'''
 * file :	MeObject.py
 * author :	bushaofeng
 * create :	2017-01-12 17:37
 * func : 
 * history:
'''

import tornado.web
import threading
import json
from BaseHandler import *
from mecloud.model.MeObject import *
from mecloud.model.MeQuery import *
from mecloud.model.MeError import *
from mecloud.helper.DbHelper import *
from mecloud.helper.ClassHelper import *
from mecloud.helper.Util import *
from mecloud.lib import *
from ConfigParser import *
import time, os, sched

access_token = None
jsapi_ticket = None
def accessTokenTask(inc = 7000): 
    global access_token
    global jsapi_ticket
    log.info("start access_token thread!")
    while True:
        access_token = wx.accessTokenFromWx()
        jsapi_ticket = wx.jsapiTicketFromWx()
        time.sleep(inc)

class WxHandler(BaseHandler):
    wx_thread = None
    def __init__(self, *args, **kwargs):
        BaseHandler.__init__(self, *args, **kwargs)
        config = ConfigParser()
        config.read('./config')
        # access_server = config.get('global', 'WX_ACCESSTOKEN_SERVER')
        access_server = self.application.config.get('wx', 'WX_ACCESSTOKEN_SERVER')
        if access_server and access_server!='0' and access_server!='False' and (not WxHandler.wx_thread):
            WxHandler.wx_thread = threading.Thread(target=accessTokenTask, args=(7000,))
            WxHandler.wx_thread.daemon = True
            WxHandler.wx_thread.start()

    def get(self, action):
        global access_token
        global jsapi_ticket
        if action=='access_token':
            if not access_token:
                time.sleep(2)
            self.write(json.dumps(access_token))
        elif action=='jsapi_ticket':
            if not jsapi_ticket:
                time.sleep(2)
            self.write(json.dumps(jsapi_ticket))
        elif action == 'login':
            # state参数带跳转url
            code = self.get_argument('code', None)
            uid = self.get_argument('user', None)
            if not code and uid:
                self.set_secure_cookie('u', uid)
                self.write(ERR_SUCCESS.message)
                return
            elif code:
                refer = unquote(self.get_argument('state'))
                token = wx.accessTokenFromCode(code)
                #user = WxHandler.getUserFromOpenid(token)
                # 如果数据库中没有表示没有关注，则邀请关注
                userQuery = MeQuery('User')
                user = userQuery.findOne({'openid': token['openid']})
                if not user:
                    user = WxHandler.getUserFromOpenid(token)
                    # self.render("error.html", user=None, wxconfig= self.wxconfig)
                    # return
                self.set_secure_cookie('u', user['_id'])
                self.redirect(refer)
            else:
                self.write(ERR_PARA.message)

        elif action=='logout':
            self.clear_cookie("u")
            self.write("退出成功")
            
    @staticmethod
    def getUserFromOpenid(token):
        userHelper = MeQuery('User')
        user = userHelper.findOne({'openid': token['openid']})
        if not user:
            user_info = wx.getSnsUserInfo(token['access_token'], token['openid'])
            if user_info!=None:
                user = MeObject("User", user_info)
                user.save()
            else:
                return None
        log.debug("redirect userinfo: %s", str(user))
        return user
    def getUserFromUnionid(unionid):
        pass
