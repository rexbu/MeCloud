# coding=utf8
import json
import traceback

from mecloud.api.BaseHandler import BaseHandler, ERR_PARA, MeUser, ERR_USERPWD_MISMATCH, MeEncoder, ClassHelper, \
    ERR_QRCODE_TO_LOGIN, ERR_USER_NOTFOUND, MeQuery, MeObject
from mecloud.helper.RedisHelper import RedisDb

from ws import Constants
from wsserver import logger


class QRCodeHandler(BaseHandler):
    # @tornado.web.authenticated
    def get(self, action=None):
        pass

    # @tornado.web.authenticated
    def post(self, action=None):
        try:
            logger.debug('action: %s', action)
            if action == 'login':
                self.login()
            else:
                print "action error: " + action
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)

    # 二维码登录　设备迁移
    def login(self):
        mid = self.jsonBody['mid']
        device = self.jsonBody['device']
        logger.debug('mid:%s', mid)
        userMigrateHelper = ClassHelper('UserMigrate')
        user_migrate = userMigrateHelper.find_one({'_sid': mid, 'isused': 0})
        logger.debug('user_migrate:%s', user_migrate)
        if not user_migrate:
            self.write(ERR_QRCODE_TO_LOGIN.message)
            return
        else:
            logger.debug('user_migrate.createAt:%s', user_migrate['createAt'])
            user = ClassHelper('User').get(user_migrate['user'])
            # user = MeUser(obj=user_obj)
            # user = MeQuery('User').get(user_migrate['user'])
            if not user:
                self.write(ERR_USER_NOTFOUND.message)
                return
            else:
                # 长链接通知
                notify_dict = {'to_id': user['_sid'], 't': 'logout'}
                message_json = json.dumps(notify_dict, ensure_ascii=False)
                logger.debug(message_json)
                # message_json. = long(message['createAt'].strftime('%s')) * 1000
                rc = RedisDb.get_connection()
                r = rc.publish(Constants.REDIS_CHANNEL_FOR_PUSH, message_json)
                logger.debug('redis publish r:%s', r)
                # del user['password']
                # del user['username']
                user['device'] = device
                user_info = ClassHelper('UserInfo').find_one({'user': user['_sid']})
                del user['createAt']
                del user['updateAt']
                del user['password']
                del user_info['createAt']
                del user_info['updateAt']
                result = {}
                result['user'] = user
                result['user_info'] = user_info
                result['errCode'] = 0
                result['info'] = 'success'
                logger.debug('result: %s', json.dumps(result))
                self.write(json.dumps(result))
                self.set_secure_cookie("u", user['_id'])
                new_cookie = str(self._new_cookie.get('u')).split('=')[1].split(';')[0].replace('"', '')
                print new_cookie
                print user['_id']
                if new_cookie is not None:
                    self.set_user_cookie_record(new_cookie, user['_id'])
                try:
                    userMigrateHelper.update(mid, {'$set': {'isused': 1}})
                    ClassHelper('User').update(user['_sid'], {'$set': {'username': device, 'device': device}})
                    self.save_login_log(user['_sid'])
                except Exception, e:
                    logger.error(e)
                    msg = traceback.format_exc()
                    logger.error(msg)

    def save_login_log(self, userid):
        try:
            client = self.request.headers.get("X-MeCloud-Client", None)
            ip = self.request.remote_ip
            device = self.request.headers.get("X-MeCloud-Device", None)
            platform = self.request.headers.get("X-MeCloud-Platform", None)
            system = self.request.headers.get("X-MeCloud-System", None)
            channel = self.request.headers.get("X-MeCloud-Channel", None)
            location_lon = self.request.headers.get("X-MeCloud-Location-Lon", None)
            location_lat = self.request.headers.get("X-MeCloud-Location-Lat", None)
            print 'client, ip, device, platform, system, channel, lon, lat ', client, ip, device, platform, system, channel, location_lon, location_lat
            ll = MeObject('LoginLog')
            ll['user'] = userid
            if client:
                ll['client'] = client
            if ip:
                ll['ip'] = ip
            if device:
                ll['device'] = device
            if platform:
                ll['platform'] = platform
            if system:
                ll['system'] = system
            if channel:
                ll['channel'] = channel
            if location_lon and location_lat:
                ll['location'] = {'lon': location_lon, 'lat': location_lat}
            ll.save()
        except Exception, e:
            print e
            msg = traceback.format_exc()
            print msg
