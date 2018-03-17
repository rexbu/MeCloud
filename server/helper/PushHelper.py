# coding=utf-8

import json

import requests

from mecloud.helper.ClassHelper import ClassHelper

# TODO
from mecloud.lib import log


class PushHelper:
    autoMessage = {}
    pushUrl = None
    def __init__(self, className, method, userId):
        self.className = className
        self.method = method
        self.userId = userId
        if not self.autoMessage:
            autoMessageClassHelper = ClassHelper("AutoMessage")
            items = autoMessageClassHelper.find({})
            for item in items:
                self.autoMessage[(item['classname'], item['method'])] = item

    def sentMessage(self, obj):
        item = self.autoMessage.get((self.className, self.method),None)
        if not item:
            return
        userClassHelper = ClassHelper("UserInfo")
        userInfo = userClassHelper.find_one({"user":self.userId},{'nickName':1})
        if not userInfo:
            return
        obj.update(userInfo)
        title = item['title'].format(**obj)
        template = item['template'].format(**obj)
        # TODO
        pushData = {"userid": obj['to'], "title": title, "text": template}
        log.info("Push Data:%s",json.dumps(pushData))
        try:
            res = requests.post(self.pushUrl, data=json.dumps(pushData), headers={'X-MeCloud-Debug': '1'})
            print res.text
        except Exception,ex:
            log.err("Push Err:%s",ex)
