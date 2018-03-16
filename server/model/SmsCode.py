# -*- coding: utf-8 -*-

import sys
from aliyunsdkdysmsapi.request.v20170525 import SendSmsRequest
from aliyunsdkdysmsapi.request.v20170525 import QuerySendDetailsRequest
from aliyunsdkcore.client import AcsClient
import uuid

class SmsCodeConfig():

    region = None
    access_key_id = None
    access_key_secret = None
    template_code = None
    sign_name = None



class SmsCode():
    reload(sys)
    sys.setdefaultencoding('utf8')


    @staticmethod
    def send_sms(phone, code):
        print SmsCodeConfig.access_key_secret

        template_param = '{"code":"' + str(code) + '","product":"必连科技"}'
        acs_client = AcsClient(SmsCodeConfig.access_key_id, SmsCodeConfig.access_key_secret, SmsCodeConfig.region)
        smsRequest = SendSmsRequest.SendSmsRequest()
        # 申请的短信模板编码,必填
        smsRequest.set_TemplateCode(SmsCodeConfig.template_code)

        # 短信模板变量参数
        smsRequest.set_TemplateParam(template_param)

        # 设置业务请求流水号，必填。
        smsRequest.set_OutId(uuid.uuid1())

        # 短信签名
        smsRequest.set_SignName(SmsCodeConfig.sign_name);

        # 短信发送的号码列表，必填。
        smsRequest.set_PhoneNumbers(phone)

        # 调用短信发送接口，返回json
        smsResponse = acs_client.do_action_with_exception(smsRequest)

        return smsResponse
