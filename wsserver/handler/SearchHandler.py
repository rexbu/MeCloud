# coding=utf8
import json
import traceback

import tornado.web
from mecloud.api.BaseHandler import BaseHandler, ERR_PARA, ClassHelper, MeObject, MeQuery
from mecloud.helper import CountHelper

from es import EsClient
from util import RedisUtil
from wsserver import logger


class SearchHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self):
        pass

    @tornado.web.authenticated
    def post(self, action=None):
        if action == 'user':
            self.searchUser()
        else:
            print "action error: " + action

    # 搜索用户
    def searchUser(self):
        try:
            logger.debug(self.jsonBody)
            obj = self.jsonBody
            content = obj.get('content')
            logger.debug('search content:%s', content)
            scroll_id = obj.get('scrollId', None)
            page_size = obj.get('pageSize', None)
            if not page_size:
                page_size = 10
            if page_size is None or page_size <= 0 or page_size > EsClient.USER_PAGE_SIZE:
                page_size = EsClient.USER_PAGE_SIZE
            # logger.debug('scroll_id: %s', scroll_id)
            # logger.debug('page_size: %d', page_size)
            if not content and not scroll_id:
                self.write(ERR_PARA.message)
            else:
                # print content.replace(' ', '')
                # es 搜索 (去空格)
                searched = EsClient.searchUser(content.replace(' ', ''), scroll_id, pageSize=page_size)
                users = []
                for item in searched['hits']['hits']:
                    source = item['_source']
                    del source['pinyin']
                    del source['pinyinsmall']
                    users.append(source)
                # print users
                logger.debug('find num: %d', users.__len__())
                result = {}
                result['scrollId'] = searched['_scroll_id']
                result['users'] = users
                if users.__len__() > 0:
                    followerHelper = ClassHelper('Followee')

                    for user in result['users']:
                        r = CountHelper.get_specific_count(user['user'])
                        logger.debug('r:%s', r)
                        logger.debug('nickName: %s,user: %s', user['nickName'], user['user'])
                        user['fansCount'] = r['followers']
                        user['followers'] = r['followers']
                        # TODO 计算来自XX人贡献了xx照片
                        user['assigners'] = r['assigners']
                        user['contributeCount'] = r['assigners']
                        user['imageCount'] = r['medias']
                        user['medias'] = r['medias']
                        user['isUser'] = 1
                        del user['id']
                # logger.debug('final result:' + json.dumps(result, ensure_ascii=False))
                result['errCode'] = 0  # 执行成功code=0
                self.write(result)
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)





            # def save_login_log(self):
            #     try:
            #         client = self.request.headers.get("X-MeCloud-Client", None)
            #         ip = self.request.remote_ip
            #         device = self.request.headers.get("X-MeCloud-Device", None)
            #         platform = self.request.headers.get("X-MeCloud-Platform", None)
            #         system = self.request.headers.get("X-MeCloud-System", None)
            #         channel = self.request.headers.get("X-MeCloud-Channel", None)
            #         location_lon = self.request.headers.get("X-MeCloud-Location-Lon", None)
            #         location_lat = self.request.headers.get("X-MeCloud-Location-Lat", None)
            #         logger.debug('client:%s, ip:%s, device:%s, platform:%s, system:%s, channel:%s, lon:%s, lat:%s ', client,
            #                      ip, device, platform, system, channel, location_lon, location_lat)
            #         userid = self.user['_id']
            #         ll = MeObject('LoginLog')
            #         ll['user'] = userid
            #         if client:
            #             ll['client'] = client
            #         if ip:
            #             ll['ip'] = ip
            #         if device:
            #             ll['device'] = device
            #         if platform:
            #             ll['platform'] = platform
            #         if system:
            #             ll['system'] = system
            #         if channel:
            #             ll['channel'] = channel
            #         if location_lon and location_lat:
            #             ll['location'] = {'lon': location_lon, 'lat': location_lat}
            #         ll.save()
            #     except Exception, e:
            #         logger.error(e)
            #         msg = traceback.format_exc()
            #         logger.error(msg)
