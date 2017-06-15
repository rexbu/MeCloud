#-*- coding: utf-8 -*-
from bson import json_util
from datetime import *
from BaseHandler import *
from CaptchaHandler import *
from model.MeError import *
from helper.DbHelper import *
from helper.ClassHelper import *
from helper.Util import *
from lib import *
from model.MeError import *
from datetime import *
import json
import hashlib
import os
import urllib
import time

class FileHandler(BaseHandler):
	def get(self, filename):
		db = self.get_argument('db', self.appDb)
		if filename=='upload':
			file = MeFile(db, self.get_argument('filename'))
			file.save()
			self.write({'_id':file.objectId, 'url': file.putUrl()})
		else:
			log.info('download file:[%s/%s]', db, filename)
			file = MeFile(db)
			file.get(filename)
			self.redirect(file.getUrl())

	@tornado.web.authenticated
	def post(self, action=None):
		pass
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

