# -*- coding: utf-8 -*-
__author__ = 'zqzhao5'
import json

class AppConditions:
    #手机类型
    PHONE_TYPE = "phoneType"

    #地区
    REGION = "region"

    #自定义tag
    TAG = "tag"

    condition = []
    def addCondition(self, key, values, optType = 0):
        item = {"key": key, "values": values, "optType": optType}
        self.condition.append(item)
        return  self

    def getCondition(self):
        return self.condition
