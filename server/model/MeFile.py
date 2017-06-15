#-*- coding: utf-8 -*- 
'''
 * file :	MeFile.py
 * author :	bushaofeng
 * create :	2016-06-15 19:27
 * func : 
 * history:
'''

from MeObject import *
import oss2
import time
import datetime
import base64
import hmac
from hashlib import sha1 as sha

class MeFile(MeObject):
	domain = None
	auth = oss2.Auth('CYZ1M9y8GMVaC1So', 'NRMFbx4qByQB43iojOqM6XsbCuvtjn')
	bucketName = None
	bucket = None
	bucketUrl = None
	# 过期时间
	expire_time = 600
	# 最大文件大小
	max_size = 10000000

	def __init__(self, name=None):
		MeObject.__init__(self, 'MeFile')
		if name!=None:
			self['name'] = name
			namesplit = name.split('.')
			if len(namesplit)>1:
				self['type'] = namesplit[len(namesplit)-1]

	### 根据id建一个空的对象
	@staticmethod
	def createWithId(oid):
		file = MeFile()
		file.setOverLoad('_id', oid)
		file.objectId = oid
		return file

	def get_iso_8601(self,expire):
	    gmt = datetime.datetime.fromtimestamp(expire).isoformat()
	    gmt += 'Z'
	    return gmt

	def postPara(self):
		now = int(time.time())
		expire_syncpoint  = now + MeFile.expire_time
		expire = self.get_iso_8601(expire_syncpoint)
		policy_dict = {}
		policy_dict['expiration'] = expire
		condition_array = [] 
		array_item = []
		array_item.append('content-length-range');
		array_item.append(1);
		array_item.append(MeFile.max_size);
		condition_array.append(array_item)
		# condition_array.append({'bucket':'vsfile'})
		policy_dict['conditions'] = condition_array
		policy = json.dumps(policy_dict).strip()
		policy_encode = base64.b64encode(policy)
		h = hmac.new('NRMFbx4qByQB43iojOqM6XsbCuvtjn', policy_encode, sha)
		signature = base64.encodestring(h.digest()).strip()
		# type = get_argument('filename').split('.')[-1]
		return {
			'policy':policy_encode, 'signature':signature,
			'fileName': self.objectId+'.'+self['type'], 'id': self.objectId,
			'access': 'CYZ1M9y8GMVaC1So',
			'bucketUrl': MeFile.bucketUrl, 
		}


	def getUrl(self):
		total = 0
		filename = self.objectId
		if self.has_key('type'):
			filename += ('.'+self['type'])
		# 刚刚上传到oss的文件可能查询不到，需要等一会
		while (not MeFile.bucket.object_exists(filename) ):
			time.sleep(1)
			total+=1
			# 最多等待3秒
			if total>=3:
				return None
		return MeFile.bucket.sign_url('GET', filename, 300)

	def getOSSFileName(self):
		filename = self.objectId
		if self.has_key('type'):
			filename += ('.'+self['type'])
		return filename




