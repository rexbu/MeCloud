# coding=utf8
import json
import traceback

import tornado.web
from mecloud.api.BaseHandler import BaseHandler, ERR_PARA, ERR_SUCCESS, MeObject, MeQuery, datetime, date, ObjectId, \
    ClassHelper, crypto, BaseConfig
from mecloud.helper.RedisHelper import RedisDb

from getui import demo2
from util import RedisUtil, DetectClass, SessionUtil
from ws import Constants
from wsserver import logger


class ConfigHandler(BaseHandler):
    # @tornado.web.authenticated
    def get(self, action=None):
        try:
            if action == 'base':
                self.base()
            # elif action == 'create':
            #     self.create()
            elif action == 'createConfig':
                self.createConfig()
            # elif action == 'createAlbumConfig':
            #     self.createAlbumConfig()
            # elif action == 'createCommentSession':
            #     self.createCommentSession()
            else:
                print 'action error ', action
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)

    # @tornado.web.authenticated
    def post(self, action=None):
        pass

    def base(self):
        version = self.request.headers.get("X-MeCloud-Client", None)
        platform = self.request.headers.get("X-MeCloud-Platform", None)
        app_version = self.request.headers.get("X-MeCloud-AppVersion", None)
        logger.debug('in header, version:%s , platform:%s, app_version:%s', version, platform, app_version)
        if not version:
            version = '1'
        if not platform:
            platform = 'android'

        cursor_config = ClassHelper('VersionControl').find(
            {'platform': platform, 'version': version, 'settingName': {'$exists': True}})
        config_list = []
        if cursor_config:
            for data in cursor_config:
                if data.has_key('createAt'):
                    del data['createAt']
                    del data['updateAt']
                    del data['version']
                    del data['_id']
                    del data['platform']
                    del data['_sid']
                    logger.debug('data:%s', data)
                config_list.append(data)
        if not config_list:
            cursor_config = ClassHelper('VersionControl').find(
                {'platform': platform, 'versionNo': int(app_version), 'settingName': {'$exists': True}})
            if cursor_config:
                for data in cursor_config:
                    if data.has_key('createAt'):
                        del data['createAt']
                        del data['updateAt']
                        del data['version']
                        del data['_id']
                        del data['platform']
                        del data['_sid']
                        logger.debug('data:%s', data)
                    config_list.append(data)
        cursor_price = ClassHelper('PriceConfig').find({}, sort={"price": 1})  # , sort={"price": 1}
        list_price = []
        if cursor_price:
            for data in cursor_price:
                # d = {'id': data['_id'], 'price': data['price']}
                list_price.append(data['price'])
        logger.debug('list_price:%s', list_price)
        r = {}
        r['configs'] = config_list
        r['prices'] = list_price
        r['code'] = 0
        r['errCode'] = 0
        logger.debug('r:%s', r)
        # if BaseConfig.mode == 'online':
        #     r = crypto.encrypt(r)
        self.write(r)

    def create(self):
        price_config = MeObject('PriceConfig')
        price = self.get_argument('price', 1)
        price_config['price'] = int(price)
        price_config.save()
        # logger.debug('')
        self.write(ERR_SUCCESS.message)

    def createConfig(self):
        o = MeObject('VersionControl')
        o['settingName'] = 'needCode'
        o['switch'] = True
        o['platform'] = 'android'
        o['version'] = '1.0'
        o['versionNo'] = 2
        o.save()
        # o = MeObject('VersionControl')
        # o['settingName'] = 'inReview'
        # o['switch'] = True
        # o['platform'] = 'android'
        # o['version'] = '1'
        # o.save()
        #
        # o = MeObject('VersionControl')
        # o['settingName'] = 'wxPay'
        # o['switch'] = True
        # o['platform'] = 'ios'
        # o['version'] = '1'
        # o.save()
        # o = MeObject('VersionControl')
        # o['settingName'] = 'wxPay'
        # o['switch'] = True
        # o['platform'] = 'android'
        # o['version'] = '1'
        # o.save()
        #
        # o = MeObject('VersionControl')
        # o['settingName'] = 'extra'
        # o['switch'] = True
        # o['platform'] = 'ios'
        # o['version'] = '1'
        # o.save()
        # o = MeObject('VersionControl')
        # o['settingName'] = 'extra'
        # o['switch'] = True
        # o['platform'] = 'android'
        # o['version'] = '1'
        # o.save()
        #
        # o = MeObject('VersionControl')
        # o['settingName'] = 'applePay'
        # o['switch'] = True
        # o['platform'] = 'ios'
        # o['version'] = '1'
        # o.save()
        # o = MeObject('VersionControl')
        # o['settingName'] = 'applePay'
        # o['switch'] = True
        # o['platform'] = 'android'
        # o['version'] = '1'
        # o.save()
        #
        # o = MeObject('VersionControl')
        # o['settingName'] = 'showIncome'
        # o['switch'] = True
        # o['platform'] = 'ios'
        # o['version'] = '1'
        # o.save()
        # o = MeObject('VersionControl')
        # o['settingName'] = 'showIncome'
        # o['switch'] = True
        # o['platform'] = 'android'
        # o['version'] = '1'
        # o.save()
        self.write(ERR_SUCCESS.message)

    def createAlbumConfig(self):
        for d in DetectClass.list:
            o = MeObject('AlbumConfig')
            o['id'] = d['id']
            o['name'] = d['name']
            o['title'] = ''
            o['weight'] = 0
            o['show'] = False
            for ud in DetectClass.list_use:
                if d['id'] == ud['id']:
                    o['title'] = ud['title']
                    o['show'] = True
                    break
            o.save()
        self.write(ERR_SUCCESS.message)

    def createCommentSession(self):
        cursor = ClassHelper('Message').find({'msg_type': 2})
        if cursor:
            i = 1
            for d in cursor:
                m = MeObject('Message', obj=d)
                logger.debug('message id :%s', m['_id'])
                logger.debug('fromid:%s toid:%s', m['from_id'], m['to_id'])
                session = SessionUtil.create(m['from_id'], m['to_id'])
                logger.debug('session:%s', session)
                m['session'] = session
                m.save()
                logger.debug('%d finish', i)
                i = i + 1
                # break
        logger.debug('all finish')
