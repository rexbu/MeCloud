# -*- coding: utf-8 -*-

import json
import tornado.web
from mecloud.helper.Util import MeEncoder
from mecloud.model.MeFile import MeFile
from mecloud.model.MeFile import MeFileConfig

class CmsFileUploadHandler(tornado.web.RequestHandler):
    # @tornado.web.authenticated
    def post(self, action=None):
        if action == "uploadFile":
            self.uploadFile()
    def uploadFile(self):
        """
                保存照片file
                :param fileId:
                :param fileBody:
                :return:
                """
        fileBody = self.request.body
        file = MeFile()
        file['type'] = 'jpg'
        file['size'] = len(fileBody)
        file['bucket'] = MeFileConfig.bucket_name
        file['platform'] = MeFileConfig.platform
        file.save()
        file['name'] = str(file['_id'])
        file.save()
        MeFile.upload_data(str(file['_id']), fileBody)
        self.write(json.dumps(file, cls=MeEncoder))
