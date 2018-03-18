# -*- coding: utf-8 -*-

from getui import RequestException
from getui.igetui.igt_target import Target
from getui.igetui.template.igt_transmission_template import TransmissionTemplate
from igetui.igt_message import IGtAppMessage
from igetui.template.igt_link_template import LinkTemplate
from igt_push import IGeTui, IGtSingleMessage, APNPayload, DictionaryAlertMsg

# 定义常量, appId、appKey、masterSecret 采用本文档 "第二步 获取访问凭证 "中获得的应用配置

# gs config
# APPID = '6AVJqp6sEA8MtOtm7iAUv9'
# APPKEY = 'ZHNBKPNYp96ZJ03hOw5QO4'
# MASTERSECRET = 'lF6nf3H5DY6QXD8rBmZys4'
# CID = '098eaa9d72b20039903cd81089384136'

APPID = 'XqGSV3Bh26AtWp7KPZpNXA'
APPKEY = 'x6gJQSIO107DHgjgXaVTE1'
MASTERSECRET = 'DnJ5lbB4fP74plK4fvMlf5'

# CID = 'd60b5a08ab02efb851508247471419b0'
CID = '70672d704c8ad6249f57371d185e58f8'

HOST = 'http://sdk.open.api.igexin.com/apiex.htm'


class GetuiPush:
    push = None


# 群发push
def pushMessageToApp():
    # print HOST
    # print CID
    if GetuiPush.push is None:
        GetuiPush.push = IGeTui(HOST, APPKEY, MASTERSECRET)
    push = GetuiPush.push
    print push.host

    # 新建一个推送模版, 以链接模板为例子，就是说在通知栏显示一条含图标、标题等的通知，用户点击可打开您指定的网页
    template = LinkTemplate()
    template.appId = APPID
    template.appKey = APPKEY
    template.title = u"欢迎使用个推 gs!"
    template.text = u"这是一条APP推送消息~"
    template.logo = ""
    template.url = "http://www.baidu.com"
    template.transmissionType = 1
    template.transmissionContent = ''
    template.isRing = True
    template.isVibrate = True
    template.isClearable = True

    # 定义"AppMessage"类型消息对象，设置消息内容模板、发送的目标App列表、是否支持离线发送、以及离线消息有效期(单位毫秒)
    message = IGtAppMessage()
    message.data = template
    message.isOffline = True
    message.offlineExpireTime = 1000 * 600
    message.appIdList.extend([APPID])

    ret = push.pushMessageToApp(message)
    print ret
    return ret


# 单独push for test
def pushMessageToSingle(cid, title, text):
    # http的接口
    # push = IGeTui(None, APPKEY, MASTERSECRET,False)
    # https的接口
    # push = IGeTui(None, APPKEY, MASTERSECRET,True)
    # 根据HOST区分是https还是http
    # push = IGeTui(HOST, APPKEY, MASTERSECRET)
    print 'appid,', APPID
    if GetuiPush.push is None:
        GetuiPush.push = IGeTui(HOST, APPKEY, MASTERSECRET)
    push = GetuiPush.push
    # 消息模版：
    # 1.TransmissionTemplate:透传功能模板
    # 2.LinkTemplate:通知打开链接功能模板
    # 3.NotificationTemplate：通知透传功能模板
    # 4.NotyPopLoadTemplate：通知弹框下载功能模板

    #     template = NotificationTemplateDemo()
    # template = LinkTemplateDemo()
    # 新建一个推送模版, 以链接模板为例子，就是说在通知栏显示一条含图标、标题等的通知，用户点击可打开您指定的网页
    template = LinkTemplate()
    template.appId = APPID
    template.appKey = APPKEY
    # template.title = u"欢迎使用个推 gs!"
    # template.text = u"这是一条single推送消息"
    template.title = title
    template.text = text
    template.logo = ""
    template.url = "http://www.baidu.com"
    template.transmissionType = 1
    template.transmissionContent = ''
    template.isRing = True
    template.isVibrate = True
    template.isClearable = True
    # template = NotyPopLoadTemplateDemo()
    #     iOS setAPNInfo
    apnpayload = APNPayload()
    apnpayload.badge = 4
    apnpayload.sound = "sound"
    apnpayload.addCustomMsg("payload", "payload")
    #     apnpayload.contentAvailable = 1
    #     apnpayload.category = "ACTIONABLE"

    alertMsg = DictionaryAlertMsg()
    alertMsg.body = 'body'
    alertMsg.actionLocKey = 'actionLockey'
    alertMsg.locKey = 'lockey'
    alertMsg.locArgs = ['locArgs']
    alertMsg.launchImage = 'launchImage'
    # iOS8.2以上版本支持
    #     alertMsg.title = 'Title'
    #     alertMsg.titleLocArgs = ['TitleLocArg']
    #     alertMsg.titleLocKey = 'TitleLocKey'
    apnpayload.alertMsg = alertMsg
    template.setApnInfo(apnpayload)

    message = IGtSingleMessage()
    message.isOffline = False
    message.offlineExpireTime = 1000 * 3600 * 12
    message.data = template
    # message.pushNetWorkType = 2

    target = Target()
    target.appId = APPID
    target.clientId = cid

    try:
        ret = push.pushMessageToSingle(message, target)
        print ret
        return ret
    except RequestException, e:
        requstId = e.getRequestId()
        ret = push.pushMessageToSingle(message, target, requstId)
        print ret


def pushMessageToSingle2():
    if GetuiPush.push is None:
        GetuiPush.push = IGeTui(HOST, APPKEY, MASTERSECRET)
    push = GetuiPush.push
    # push = IGeTui(HOST, APPKEY, MASTERSECRET)
    # push = IGeTui("",APPKEY,MASTERSECRET)#此方式可通过获取服务端地址列表判断最快域名后进行消息推送，每10分钟检查一次最快域名
    # 消息模版：
    # TransmissionTemplate:透传功能模板，定义透传内容，应用启动形式
    template = TransmissionTemplateDemo()
    # 定义"SingleMessage"消息体，设置是否离线，离线有效时间，模板设置
    message = IGtSingleMessage()
    message.isOffline = True
    message.offlineExpireTime = 1000 * 3600 * 12
    message.data = template
    message.pushNetWorkType = 0  # 设置是否根据WIFI推送消息，2为4G/3G/2G,1为wifi推送，0为不限制推送
    target = Target()
    target.appId = APPID
    target.clientId = CID
    # target.alias = ALIAS

    try:
        ret = push.pushMessageToSingle(message, target)
        print ret
    except RequestException, e:
        # 发生异常重新发送
        requstId = e.getRequestId()
        ret = push.pushMessageToSingle(message, target, requstId)
        print ret


# 透传模板动作内容
def TransmissionTemplateDemo():
    template = TransmissionTemplate()
    template.transmissionType = 1
    template.appId = APPID
    template.appKey = APPKEY
    template.transmissionContent = '请输入您要透传内容'
    #     iOS setAPNInfo
    apnpayload = APNPayload()
    apnpayload.badge = 4
    apnpayload.sound = "sound"
    apnpayload.addCustomMsg("payload", "payload")
    #     apnpayload.contentAvailable = 1
    #     apnpayload.category = "ACTIONABLE"

    alertMsg = DictionaryAlertMsg()
    alertMsg.body = 'body'
    alertMsg.actionLocKey = 'actionLockey'
    alertMsg.locKey = 'lockey'
    alertMsg.locArgs = ['locArgs']
    alertMsg.launchImage = 'launchImage'
    # iOS8.2以上版本支持
    alertMsg.title = 'Title'
    alertMsg.titleLocArgs = ['TitleLocArg']
    alertMsg.titleLocKey = 'TitleLocKey'
    apnpayload.alertMsg = alertMsg
    template.setApnInfo(apnpayload)

    # 设置通知定时展示时间，结束时间与开始时间相差需大于6分钟（误差6分钟），消息推送后，客户端将在指定时间差内展示消息
    # begin = "2015-03-04 17:40:22";
    # end = "2015-03-04 17:47:24";
    # template.setDuration(begin, end)
    return template


def pushMessageToSingle3(cid, title, text):
    # http的接口
    # push = IGeTui(None, APPKEY, MASTERSECRET,False)
    # https的接口
    # push = IGeTui(None, APPKEY, MASTERSECRET,True)
    # 根据HOST区分是https还是http
    # push = IGeTui(HOST, APPKEY, MASTERSECRET)
    print 'appid,', APPID
    if GetuiPush.push is None:
        GetuiPush.push = IGeTui(HOST, APPKEY, MASTERSECRET)
    push = GetuiPush.push
    # 消息模版：
    # 1.TransmissionTemplate:透传功能模板
    # 2.LinkTemplate:通知打开链接功能模板
    # 3.NotificationTemplate：通知透传功能模板
    # 4.NotyPopLoadTemplate：通知弹框下载功能模板

    #     template = NotificationTemplateDemo()
    # template = LinkTemplateDemo()
    # 新建一个推送模版, 以链接模板为例子，就是说在通知栏显示一条含图标、标题等的通知，用户点击可打开您指定的网页
    template = TransmissionTemplate()
    template.transmissionType = 1
    template.appId = APPID
    template.appKey = APPKEY
    template.transmissionContent = '请输入您要透传内容'
    #     iOS setAPNInfo
    apnpayload = APNPayload()
    apnpayload.badge = 4
    apnpayload.sound = "sound"
    apnpayload.addCustomMsg("payload", "payload")
    #     apnpayload.contentAvailable = 1
    #     apnpayload.category = "ACTIONABLE"

    alertMsg = DictionaryAlertMsg()
    alertMsg.body = 'body'
    alertMsg.actionLocKey = 'actionLockey'
    alertMsg.locKey = 'lockey'
    alertMsg.locArgs = ['locArgs']
    alertMsg.launchImage = 'launchImage'
    # iOS8.2以上版本支持
    alertMsg.title = 'Title'
    alertMsg.titleLocArgs = ['TitleLocArg']
    alertMsg.titleLocKey = 'TitleLocKey'
    apnpayload.alertMsg = alertMsg
    template.setApnInfo(apnpayload)

    message = IGtSingleMessage()
    message.isOffline = False
    message.offlineExpireTime = 1000 * 3600 * 12
    message.data = template
    # message.pushNetWorkType = 2

    target = Target()
    target.appId = APPID
    target.clientId = cid

    try:
        ret = push.pushMessageToSingle(message, target)
        print ret
        return ret
    except RequestException, e:
        requstId = e.getRequestId()
        ret = push.pushMessageToSingle(message, target, requstId)
        print ret


if __name__ == '__main__':
    # pushMessageToApp()
    pushMessageToSingle(CID, u'heimi_push_test', '中文123'.decode("utf-8"))
    # pushMessageToSingle2()
    # pushMessageToSingle3(CID, u'heimi_push_test', u'中文123')
