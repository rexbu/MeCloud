# coding=utf-8
import time
import traceback

from handler.WsHandler import WsHandler
from util.LoggerUtil import logger
from ws import Pool


def check():
    while True:
        logger.debug('check ping')
        logger.debug('ws handler num is below:')
        logger.debug(Pool.user_dict.__len__())
        if Pool.user_dict.__len__() > 0:
            for k, v in Pool.user_dict.items():
                # print k, v
                try:
                    v.write_message('1')
                except Exception, e:
                    logger.error(e)
                    msg = traceback.format_exc()  # 方式1
                    logger.error(msg)
                    WsHandler.remove_from_dict(k)
        time.sleep(10)
