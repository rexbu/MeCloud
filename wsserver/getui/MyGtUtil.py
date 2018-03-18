# -*- coding: utf-8 -*-
__author__ = 'wei'
from igetui.igt_message import *
from igetui.igt_target import *
from igetui.template.igt_transmission_template import *
from igt_push import *

# 采用"Python SDK 快速入门"， "第二步 获取访问凭证 "中获得的应用配置
# APPKEY = "Mjv706pTKt5cTcjtqaToz8"
# APPID = "JroCkPGgpF6LzFQqqoWlhA"
# MASTERSECRET = "uIBtmad7RK706cy5MKdfp3"
# CID = "e560b884d8d9bf5bc5a0f9da545a11f3"

APPID = 'XqGSV3Bh26AtWp7KPZpNXA'
APPKEY = 'x6gJQSIO107DHgjgXaVTE1'
MASTERSECRET = 'DnJ5lbB4fP74plK4fvMlf5'
# CID = 'd60b5a08ab02efb851508247471419b0'
CID = 'fd01aa2075def70eae28f576aae7718e'

# 别名推送方式
# ALIAS = "";
HOST = 'http://sdk.open.api.igexin.com/apiex.htm'

push = None


class GetuiPush:
    push = None


def pushMessageToSingle(cid, title, content, data, badge):
    print 'cid, title, content, data:', cid, title, content, data
    if GetuiPush.push is None:
        GetuiPush.push = IGeTui(HOST, APPKEY, MASTERSECRET)
    push = GetuiPush.push
    # print push.host

    # push = IGeTui(HOST, APPKEY, MASTERSECRET)
    # push = IGeTui("",APPKEY,MASTERSECRET)#此方式可通过获取服务端地址列表判断最快域名后进行消息推送，每10分钟检查一次最快域名
    # 消息模版：
    # TransmissionTemplate:透传功能模板，定义透传内容，应用启动形式
    template = TransmissionTemplateDemo(title, content, data, badge)
    # 定义"SingleMessage"消息体，设置是否离线，离线有效时间，模板设置
    message = IGtSingleMessage()
    message.isOffline = True
    message.offlineExpireTime = 1000 * 3600 * 12
    message.data = template
    message.pushNetWorkType = 0  # 设置是否根据WIFI推送消息，2为4G/3G/2G,1为wifi推送，0为不限制推送
    target = Target()
    target.appId = APPID
    target.clientId = cid
    # target.alias = ALIAS

    try:
        ret = push.pushMessageToSingle(message, target)
        print ret
        return ret
    except RequestException, e:
        # 发生异常重新发送
        requstId = e.getRequestId()
        ret = push.pushMessageToSingle(message, target, requstId)
        print ret


# 透传模板动作内容
def TransmissionTemplateDemo(title, content, data, badge):
    if not badge:
        badge = 0
    template = TransmissionTemplate()
    template.transmissionType = 2
    template.appId = APPID
    template.appKey = APPKEY
    template.transmissionContent = '{"title":"' + title + '","content":"' + content + '","uri":"' + data + '"}'
    template.title = title
    template.text = content
    template.logo = ""
    # iOS setAPNInfo
    apnpayload = APNPayload()
    apnpayload.badge = badge
    apnpayload.sound = "sound"
    apnpayload.addCustomMsg("payload", "payload")
    apnpayload.contentAvailable = 1
    apnpayload.category = "ACTIONABLE"

    alertMsg = DictionaryAlertMsg()
    alertMsg.body = content
    alertMsg.actionLocKey = 'actionLockey'
    alertMsg.locKey = 'lockey'
    alertMsg.locArgs = ['locArgs']
    alertMsg.launchImage = 'launchImage'
    # iOS8.2以上版本支持
    alertMsg.title = title
    alertMsg.titleLocArgs = ['TitleLocArg']
    alertMsg.titleLocKey = 'TitleLocKey'
    apnpayload.alertMsg = alertMsg
    apnpayload.addCustomMsg("uri", data)
    template.setApnInfo(apnpayload)

    # 设置通知定时展示时间，结束时间与开始时间相差需大于6分钟（误差6分钟），消息推送后，客户端将在指定时间差内展示消息
    # begin = "2015-03-04 17:40:22";
    # end = "2015-03-04 17:47:24";
    # template.setDuration(begin, end)
    return template


# pushMessageToSingle()
if __name__ == '__main__':
    print pushMessageToSingle(CID, u'gs', u'this is a test', 'uri')
