# coding=utf8
import traceback

import tornado.web
from mecloud.helper.ClassHelper import ClassHelper

from util import payUtil
from util.WxPaySDK import Wxpay_server_pub
from wsserver import logger


# 微信支付回调
class WxCallbackHandler(tornado.web.RequestHandler):
    def get(self, action=None):
        print 'no method'
        pass

    def post(self):
        try:
            xml = self.request.body
            logger.debug('xml: %s', xml)
            pub = Wxpay_server_pub()
            pub.saveData(xml)
            logger.debug('pub.data: %s', pub.data)
            if pub.data['return_code'] == 'SUCCESS' and pub.data['result_code'] == 'SUCCESS':
                print 'wx call back result is success'
                order_no = pub.data.get('out_trade_no', None)
                logger.debug('out_trade_no:%s', order_no)
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
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
        self.write('<xml><return_code><![CDATA[SUCCESS]]></return_code><return_msg><![CDATA[OK]]></return_msg></xml>')
