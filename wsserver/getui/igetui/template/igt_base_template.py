# -*- coding: utf-8 -*-

import time
from getui.protobuf import *
from getui.payload.APNPayload import APNPayload, DictionaryAlertMsg


class BaseTemplate:
    def __init__(self):
        self.appKey = ""
        self.appId = ""
        self.pushInfo = gt_req_pb2.PushInfo()
        self.pushInfo.invalidAPN = True
        self.pushInfo.invalidMPN = True
        self.duration = 0
        
    def getTransparent(self):
        transparent = gt_req_pb2.Transparent()
        transparent.id = ""
        transparent.action = "pushmessage"
        transparent.taskId = ""
        transparent.appKey = self.appKey
        transparent.appId = self.appId
        transparent.messageId = ""
        transparent.pushInfo.CopyFrom(self.getPushInfo())
  
        actionChains = self.getActionChains()        
        for actionChain in actionChains:
            tmp = transparent.actionChain.add()
            tmp.CopyFrom(actionChain)           
        # add condition
        transparent.condition.append(self.getDurCondition())            
        return transparent
        
    def getActionChains(self):
        return []

    def getPushInfo(self):
        return self.pushInfo

    def setApnInfo(self, payload):
        if payload is None:
            return
        payload = payload.getPayload()
        if payload is None or payload is "":
            return
        length = len(payload)
        if length > APNPayload.PAYLOAD_MAX_BYTES:
            raise Exception("APN payload length overlength (" + str(length) + ">"
                            + str(APNPayload.PAYLOAD_MAX_BYTES) + ")")

        self.pushInfo.apnJson = payload
        self.pushInfo.invalidAPN = False

    def setPushInfo(self, actionLocKey, badge, message, sound, payload, locKey, locArgs, launchImage,
                    contentAvailable=0):

        self.pushInfo = gt_req_pb2.PushInfo()
        self.pushInfo.invalidAPN = True
        self.pushInfo.invalidMPN = True
        
        alertMsg = DictionaryAlertMsg()
        if locKey is not None and locKey is not "":
            alertMsg.locKey = locKey.decode("utf-8")
        if locArgs is not None and locArgs is not "":
            alertMsg.locArgs.append(locArgs.decode("utf-8"))
        if actionLocKey is not None and actionLocKey is not "":
            alertMsg.actionLocKey = actionLocKey.decode("utf-8")
        if message is not None and message is not "":
            alertMsg.body = message.decode("utf-8")
        if launchImage is not None and launchImage is not "":
            alertMsg.launchImage = launchImage.decode("utf-8")
        
        apn = APNPayload()
        apn.alertMsg = alertMsg
        if badge is not None:
            alertMsg.badge = badge
        if sound is not None and sound is not "":
            apn.sound = sound.decode("utf-8")
        if contentAvailable is not None:
            apn.contentAvailable = contentAvailable
        if payload is not None and payload is not "":
            apn.addCustomMsg("payload", payload.decode("utf-8"))
            
        self.setApnInfo(apn)

    def getDurCondition(self):
        return "duration=" + str(self.getDuration())
    
    def getDuration(self):
            return self.duration

    def setDuration(self, begin, end):
        s = long(time.mktime(time.strptime(begin, "%Y-%m-%d %H:%M:%S")) * 1000)
        e = long(time.mktime(time.strptime(end, "%Y-%m-%d %H:%M:%S")) * 1000)
        if s <= 0 or e <= 0:
            raise ValueError("DateFormat: yyyy-MM-dd HH:mm:ss")
        if s > e:
            raise ValueError("startTime should be smaller than endTime")
        self.duration = str(s) + "-" + str(e)
