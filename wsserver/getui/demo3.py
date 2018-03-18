# -*- coding: utf-8 -*-
from getui import RequestException
from getui.igetui.igt_target import Target
from igetui.igt_message import IGtAppMessage
from igetui.template.igt_link_template import LinkTemplate
from igt_push import IGeTui, IGtSingleMessage, APNPayload, DictionaryAlertMsg

# 定义常量, appId、appKey、masterSecret 采用本文档 "第二步 获取访问凭证 "中获得的应用配置
APPID = '6AVJqp6sEA8MtOtm7iAUv9'
APPKEY = 'ZHNBKPNYp96ZJ03hOw5QO4'
MASTERSECRET = 'lF6nf3H5DY6QXD8rBmZys4'
CID = '098eaa9d72b20039903cd81089384136'
HOST = 'http://sdk.open.api.igexin.com/apiex.htm'

push = None


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


# 单独push
def pushMessageToSingle():
    # http的接口
    # push = IGeTui(None, APPKEY, MASTERSECRET,False)
    # https的接口
    # push = IGeTui(None, APPKEY, MASTERSECRET,True)
    # 根据HOST区分是https还是http
    push = IGeTui(HOST, APPKEY, MASTERSECRET)
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
    template.title = u"欢迎使用个推 gs!"
    template.text = u"这是一条single推送消息"
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
    message.isOffline = True
    message.offlineExpireTime = 1000 * 3600 * 12
    message.data = template
    # message.pushNetWorkType = 2

    target = Target()
    target.appId = APPID
    target.clientId = CID

    try:
        ret = push.pushMessageToSingle(message, target)
        print ret
    except RequestException, e:
        requstId = e.getRequestId()
        ret = push.pushMessageToSingle(message, target, requstId)
        print ret

        # pushMessageToApp()


if __name__ == '__main__':
    # pushMessageToApp()
    pushMessageToSingle()
