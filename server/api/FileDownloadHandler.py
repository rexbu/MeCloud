# -*- coding: utf-8 -*-
import urllib2
import tornado.web
from bson import ObjectId

from mecloud.api.BaseHandler import BaseHandler, BaseConfig
from mecloud.lib import crypto, log
from mecloud.model.MeError import ERR_OBJECTID_MIS
from mecloud.model.MeFile import MeFileConfig
from mecloud.model.MeQuery import MeQuery
import oss2
from mecloud.model.SmsCode import SmsCodeConfig


class FileDownloadHandler(BaseHandler):
    isImage = False

    def get(self, objectId):
        try:
            ObjectId(objectId)
        except Exception:
            return self.write(ERR_OBJECTID_MIS.message)
        fileQuery = MeQuery("File")
        # log.info("Download objectId one: %s",objectId)
        file = fileQuery.get(objectId)
        if file == None:
            self.write(ERR_OBJECTID_MIS.message)
        else:
            self.isImage = True
            self.getImage(file)

    @tornado.web.asynchronous
    def getImage(self, file):

        self.isImage = True
        if self.get_current_user() in BaseConfig.adminUser:
            self.isImage = False
        process = None
        if self.request.arguments.has_key('x-oss-process'):
            process = self.get_argument("x-oss-process")
        # print 'SmsCodeConfig.access_key_id:', SmsCodeConfig.access_key_id
        # print 'SmsCodeConfig.access_key_secret:', SmsCodeConfig.access_key_secret
        # print 'MeFileConfig.region_id', MeFileConfig.region_id
        # print 'file bucket:', file['bucket']
        # print 'file name:', file["name"]
        auth = oss2.Auth(SmsCodeConfig.access_key_id, SmsCodeConfig.access_key_secret)
        bucket = oss2.Bucket(auth, "oss-" + MeFileConfig.region_id + ".aliyuncs.com", file['bucket'])
        result = bucket.get_object(file["name"], process=process)
        data = result.read()
        self.write(data)
        self.finish()

    def write(self, msg):
        if not self.isImage:
            BaseHandler.write(self, msg)
        else:
            tornado.web.RequestHandler.write(self, str(crypto.imageEncrypt(msg)))
