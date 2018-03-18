# from handler.WsHandler import WsHandler
from mecloud.lib import crypto

from util.LoggerUtil import logger
from ws import Pool


def execute(from_handler):
    obj = from_handler.dict_body
    to_handler = Pool.user_dict.get(str(obj['to_id']))
    if to_handler:
        del obj['to_id']
        obj['from_id'] = to_handler.userid
        logger.debug(' handler exists')
        to_handler.write_message(obj)
    else:
        logger.debug(' handler is None')
