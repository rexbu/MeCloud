# coding=utf8
import traceback

import tornado.web
from mecloud.helper.ClassHelper import ClassHelper

from mecloud.lib import payUtil
from mecloud.lib.WxPaySDK import Wxpay_server_pub


# 微信支付回调
from mecloud.lib import log


class WxCallbackHandler(tornado.web.RequestHandler):
    def get(self, action=None):
        print 'no method'
        pass

    def post(self):
        try:
            xml = self.request.body
            log.debug('xml: %s', xml)
            pub = Wxpay_server_pub()
            pub.saveData(xml)
            log.debug('pub.data: %s', pub.data)
            if pub.data['return_code'] == 'SUCCESS' and pub.data['result_code'] == 'SUCCESS':
                print 'wx call back result is success'
                order_no = pub.data.get('out_trade_no', None)
                log.debug('out_trade_no:%s', order_no)
                if order_no:
                    flow = ClassHelper("RechargeFlow").find_one({'orderNo': order_no, 'status': 0})
                    if flow:
                        payUtil.orderCallback(flow['_id'], flow['user'], 1, pub.data)
            else:
                order_no = pub.data.get('out_trade_no', None)
                if order_no:
                    flow = ClassHelper("RechargeFlow").find_one({'orderNo': order_no, 'status': 0})
                    if flow:
                        payUtil.orderCallback(flow['_id'], flow['user'], -1, pub.data)
        except Exception, e:
            log.err(e)
            msg = traceback.format_exc()
            log.err(msg)
        self.write('<xml><return_code><![CDATA[SUCCESS]]></return_code><return_msg><![CDATA[OK]]></return_msg></xml>')
