# -*- coding: utf-8 -*-
import json
import tornado.web
from mecloud.api.BaseHandler import BaseHandler
from mecloud.helper.Util import MeEncoder
from mecloud.model.MeError import ERR_PARA, ERR_INVALID
from mecloud.model.MeFile import MeFile
from mecloud.model.MeFile import MeFileConfig


class FileHandler(BaseHandler):

    isImaget = False

    @tornado.web.authenticated
    def get(self, action=None):
        if action == 'token':
            file = MeFile()
            token = file.fetch_sts_token()
            self.write(json.dumps(token, cls=MeEncoder))
        else:
            self.write(ERR_PARA.message)


                # if self.request.arguments.has_key('object_id'):
                #     objectId = self.get_argument('object_id')
                #     try:
                #         ObjectId(objectId)
                #     except Exception:
                #         return self.write(ERR_PARA.message)
                #     fileQuery = MeQuery("File")
                #     file = fileQuery.get(objectId)
                #     if file == None:
                #         self.write(ERR_OBJECTID_MIS.message)
                #     self.write(json.dumps(file, cls=MeEncoder))
                # else:
                #     return self.write(ERR_PARA.message)

    @tornado.web.authenticated
    def post(self, action=None):
        if action == 'upload':
            self.upload()
        elif action == 'uploadFile':
            self.uploadFile()
        else:
            return self.write(ERR_PARA.message)

    def upload(self):
        if not self.jsonBody.has_key('name'):
            self.write(ERR_PARA.message)
            return
        obj = self.check_field("File",self.jsonBody)
        if not obj:
            return
        file = MeFile(obj)
        if file['name'] == None:
            self.write(ERR_PARA.message)
            return
        try:
            file.upload()
            self.write(json.dumps(file, cls=MeEncoder))
        except Exception, e:
            print e
            self.write(ERR_INVALID.message)


    def uploadFile(self):
        if not self.jsonBody.has_key('data'):
            self.write(ERR_PARA.message)
        data = None
        if "data" in self.jsonBody:
            data = self.jsonBody.pop("data", None)
        obj = self.check_field("File", self.jsonBody)
        if not obj:
            return
        file = MeFile(obj)
        file['bucket'] = MeFileConfig.bucket_name
        file['platform'] = MeFileConfig.platform
        file.save()
        if file:
            name = file['_id'] + "." + file['type']
            file['name'] = name
            file.save()
            file = self.filter_field(file)
            file.upload_data(file['name'],data)
            self.write(json.dumps(file, cls=MeEncoder))
        else:
            self.write(ERR_INVALID.message)

            # upload_path=os.path.join(os.path.dirname(__file__),'../files')
            # '''
            # 	self.request.files例子：
            # 	{u'icon': [{'body': '文件内容', 'content_type': u'image/png', 'filename': u'logo-32.png'}]}
            # '''
            # for fileName in self.request.files:
            # 	file_meta = self.request.files[fileName][0]
            # 	#file_type = file_meta['content_type'].split('/')[1]
            # 	#filepath=os.path.join(upload_path, self.current_user+'_'+fileName+'.'+file_type)
            # 	save_file_name = self.current_user +'_'+datetime.now().strftime('%Y%m%d%H%M%S%f%Z')+'_'+file_meta['filename'];
            # 	filepath=os.path.join(upload_path, save_file_name)
            # 	log.info('user[%s] upload file[%s_%s]', self.current_user, fileName, file_meta['filename'])
            # 	with open(filepath,'wb') as up:
            # 		up.write(file_meta['body'])
            # 	self.write({'url': 'http://'+self.request.host+'/file/'+ save_file_name })
            # 	'''
            # 	file_meta = self.request.files[fileName][0]
            # 	#file_type = file_meta['content_type'].split('/')[1]
            # 	#filepath=os.path.join(upload_path, self.current_user+'_'+fileName+'.'+file_type)
            # 	save_file_name = self.current_user +'_'+datetime.now().strftime('%Y%m%d%H%M%S%f%Z')+'_'+file_meta['filename'];
            # 	filepath=os.path.join(upload_path, save_file_name)
            # 	log.info('user[%s] upload file[%s_%s]', self.current_user, fileName, file_meta['filename'])

            # 	# domain = 'file.visualogies.com'  阿里云用自定义域名有bug
            # 	#domain = 'oss-cn-beijing-internal.aliyuncs.com'
            # 	domain = 'oss-cn-beijing.aliyuncs.com'
            # 	auth = oss2.Auth('CYZ1M9y8GMVaC1So', 'NRMFbx4qByQB43iojOqM6XsbCuvtjn')
            # 	bucket = oss2.Bucket(auth, domain, 'vsfile')
            # 	bucket.put_object(save_file_name, file_meta['body'])
            # 	self.write({'url': 'http://'+self.request.host+'/file/'+ save_file_name })
            # 	'''
