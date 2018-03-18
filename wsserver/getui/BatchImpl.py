# -*- coding: utf-8 -*-
__author__ = 'Administrator'
import uuid
import base64
import json
import os
from protobuf import *
from GtConfig import GtConfig


class BatchImpl:
    def __init__(self, appKey, push):
        self.batchId = str(uuid.uuid1())
        self.APPKEY = appKey
        self.push = push
        self.seqId = 0
        self.innerMsgList = list()
        self.lastPostData = None

    def setApiUrl(self, url):
        pass

    def getBatchId(self):
        return self.batchId

    def add(self, message, target):
        if self.seqId >= 5000:
            raise Exception("Can not add over 5000 message once! Please call submit() first.")
        else:
            json = self.createPostParams(message, target)
            item = gt_req_pb2.SingleBatchItem()
            item.seqId = self.seqId
            item.data = json
            self.innerMsgList.append(item)
            self.seqId += 1
        return self.seqId

    def createPostParams(self, message, target):
        params = dict()
        params['action'] = "pushMessageToSingleAction"
        params['appkey'] = self.APPKEY
        transparent = message.data.getTransparent()
        params['clientData'] = base64.encodestring(transparent.SerializeToString())
        params['transmissionContent'] = message.data.transmissionContent
        params['isOffline'] = message.isOffline
        params['offlineExpireTime'] = message.offlineExpireTime
        # 增加pushNetWorkType参数(0:不限;1:wifi;2:4G/3G/2G)
        params["pushNetWorkType"] = message.pushNetWorkType
        params['appId'] = target.appId
        params['clientId'] = target.clientId
        params['type'] = 2 #default is message
        params['pushType'] = message.data.pushType
        params['version'] = '3.0.0.0'
        return json.dumps(params)

    def submit(self):
        requestId = str(uuid.uuid1())
        self.seqId = 0

        data = dict()
        data['requestId'] = requestId
        data['appkey'] = self.APPKEY
        data['action'] = 'pushMessageToSingleBatchAction'
        data['serialize'] = 'pb'
        data['async'] = GtConfig.isPushSingleBatchAsync()

        try:
            request = gt_req_pb2.SingleBatchRequest()
            request.batchId = self.batchId
            for msg in self.innerMsgList:
                tmp = request.batchItem.add()
                tmp.CopyFrom(msg)

            data["singleDatas"] = base64.encodestring(request.SerializeToString())
            self.lastPostData = data
            self.innerMsgList = []
            return self.push.httpPostJson(self.push.host, data, True)
        except:
            raise Exception("submit single batch request failed")

    def retry(self):
        if self.lastPostData is not None:
            return self.push.httpPostJson(self.push.host, self.lastPostData, True)
        return None