# coding=utf-8
import json

from mecloud.helper.RedisHelper import RedisDb
from mecloud.lib import crypto

from util.LoggerUtil import logger
from ws import Constants, Pool


def sub_pub_start():
    rc = RedisDb.get_connection()
    ps = rc.pubsub()
    ps.subscribe([Constants.REDIS_CHANNEL_FOR_PUSH])

    for item in ps.listen():
        try:
            if item['type'] == 'message':
                logger.debug('channel:' + item['channel'] + ' receive a message:')
                message_json = item['data']
                logger.debug(message_json)
                obj = json.loads(message_json)
                # logger.debug(obj)
                # logger.debug(obj.get('from_id'))
                # logger.debug(obj.get('to_id'))
                # logger.debug(s.split('_')[0])
                # logger.debug(s.split('_')[1])
                logger.debug(Pool.user_dict.__len__())
                handler = Pool.user_dict.get(str(obj.get('to_id')))
                if handler:
                    logger.debug(' handler exists')
                    # handler.write_message(crypto.encrypt(message_json))
                    handler.write_message(message_json)
                else:
                    logger.debug(' handler is None')
        except Exception as e:
            logger.error(e)
