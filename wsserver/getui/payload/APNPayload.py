# -*- coding: utf-8 -*-
__author__ = 'Administrator'

import json

class AlertMsg:
    def __init__(self):
        pass
    
    def getAlertMsg(self):
        pass
    
class DictionaryAlertMsg(AlertMsg):
    def __init__(self):
        self.title = None
        self.body = None
        self.titleLocKey = None
        self.titleLocArgs = list()
        self.actionLocKey = None
        self.locKey = None
        self.locArgs = list()
        self.launchImage = None
    
    def getAlertMsg(self):
        alertMap = dict()
        if self.title is not None and self.title is not "":
            alertMap["title"] = self.title
        if self.body is not None and self.body is not "":
            alertMap["body"] = self.body
        if self.titleLocKey is not None and self.titleLocKey is not "":
            alertMap["title-loc-key"] = self.titleLocKey
        if len(self.titleLocArgs) > 0:
            alertMap["title-loc-args"] = self.titleLocArgs
        if self.actionLocKey is not None and self.actionLocKey is not "":
            alertMap["action-loc-key"] = self.actionLocKey
        if self.locKey is not None and self.locKey is not "":
            alertMap["loc-key"] = self.locKey
        if len(self.locArgs) > 0:
            alertMap["loc-args"] = self.locArgs
        if self.launchImage is not None and self.launchImage is not "":
            alertMap["launch-image"] = self.launchImage
        return alertMap;
    
class SimpleAlertMsg(AlertMsg):
    def __init__(self):
        self.alertMsg = None
        
    def getAlertMsg(self):
        return self.alertMsg
    
class APNPayload:
        
    PAYLOAD_MAX_BYTES = 2048

    def __init__(self):
        self.APN_SOUND_SILENCE = "com.gexin.ios.silence"
        self.alertMsg = None
        self.badge = -1
        self.sound = "default"
        self.contentAvailable = 0
        self.category = None
        self.customMsg = dict()
        
    def getPayload(self):
        try:
            apsMap = dict()
            if self.alertMsg is not None and isinstance(self.alertMsg, AlertMsg):
                msg = self.alertMsg.getAlertMsg();
                if msg is not None and len(msg) > 0 :
                    apsMap["alert"] = self.alertMsg.getAlertMsg()
            if self.badge >= 0:
                apsMap["badge"] = self.badge
            if self.APN_SOUND_SILENCE!=self.sound:
                if self.sound is not None and self.sound is not "":
                    apsMap["sound"] = self.sound
                else:
                    apsMap["sound"] = "default"
            if len(apsMap) <= 0:
                raise Exception("format error")
            if self.contentAvailable > 0:
                apsMap["content-available"] = self.contentAvailable
            if self.category is not None and self.category is not "":
                apsMap["category"] = self.category

            tmp = dict()
            for key, value in self.customMsg.items():
                tmp[key] = value

            tmp["aps"] = apsMap
            return json.dumps(tmp)
        except Exception as e:
            raise Exception("create apn payload error", e)

    def addCustomMsg(self, key, value):
        if key is not None and key is not "" and value is not None:
            self.customMsg[key] = value
