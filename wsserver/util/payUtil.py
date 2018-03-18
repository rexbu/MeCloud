# coding=utf-8

from mecloud.helper.ClassHelper import ClassHelper
from mecloud.model.MeError import ERR_SUCCESS

from util.LoggerUtil import logger


def orderCallback(oId, userId, status, order):
    logger.debug('oId:%s, userId:%s, status:%d, order:%s', oId, userId, status, order)
    '''
    根据支付结果更新订单的状态
    :param oId:RechargeFlow Id
    :param userId:用户Id
    :param status: 支付是否成功，1为成功，0为失败
    :param order:第三方平台返回订单信息，包括错误码
    :return: 
    '''
    item = {
        "$set": {
            "status": status,
            "order": order
        }
    }
    ###更新充值流水记录
    orderHelper = ClassHelper("RechargeFlow")
    rechargeFlow = orderHelper.update(oId, item)
    if rechargeFlow and status == 1:
        ###更新钱包
        walletHelper = ClassHelper("Wallet")
        walletInfo = walletHelper.find_one({"user": userId})
        if walletInfo:
            wallet = {
                "$inc": {'balance': rechargeFlow['amount']}
            }
            wallet = walletHelper.update(walletInfo['_id'], wallet)
        else:
            wallet = {"user": userId, 'balance': rechargeFlow['amount']}
            walletHelper.create(wallet)
        if wallet:
            return wallet.update(rechargeFlow)
        else:
            return None
    else:
        return None
