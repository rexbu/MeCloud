# -*- coding: utf-8 -*-
from igetui.igt_message import IGtAppMessage
from igetui.template.igt_link_template import LinkTemplate
from igt_push import IGeTui

# 定义常量, appId、appKey、masterSecret 采用本文档 "第二步 获取访问凭证 "中获得的应用配置
APPID = 'JroCkPGgpF6LzFQqqoWlhA'
APPKEY = 'Mjv706pTKt5cTcjtqaToz8'
MASTERSECRET = 'uIBtmad7RK706cy5MKdfp3'
CID = 'e560b884d8d9bf5bc5a0f9da545a11f3'
HOST = 'http://sdk.open.api.igexin.com/apiex.htm'

push = None


class GetuiPush:
    push = None


def pushMessageToApp():
    # print HOST
    print CID
    if GetuiPush.push is None:
        GetuiPush.push = IGeTui(HOST, APPKEY, MASTERSECRET)
    push = GetuiPush.push
    print push.host

    # 新建一个推送模版, 以链接模板为例子，就是说在通知栏显示一条含图标、标题等的通知，用户点击可打开您指定的网页
    template = LinkTemplate()
    template.appId = APPID
    template.appKey = APPKEY
    template.title = u"欢迎使用个推!"
    template.text = u"这是一条推送消息~"
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

    # pushMessageToApp()
if __name__ == '__main__':
    pushMessageToApp()
