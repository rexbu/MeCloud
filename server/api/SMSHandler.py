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
import json
import hashlib
import os
import urllib
import socket
import time
import random
import re
import binascii

socket.setdefaulttimeout(10)
class SMSHandler(BaseHandler):
	account = 'Sj-001'
	pwd = 'Txb123456'
	template = '您正在使用视诀开发者认证，您的验证码是:%d,请在10分钟内完成验证。'
	captchaMap = {}

	@tornado.web.authenticated
	def get(self, phone):
		captcha = random.randint(100000, 999999);
		msg = SMSHandler.template%captcha;
		SMSHandler.captchaMap[phone] = (captcha, time.time());
		
		url = 'http://222.73.117.156/msg/HttpBatchSendSM?account='+SMSHandler.account+'&pswd='+SMSHandler.pwd+'&mobile='+phone+'&msg='+urllib.quote(msg)
		#TODO: 需要检查发送频率，不能再class内部添加成员变量
		try:
			res = urllib.urlopen(url).read()
		except Exception,e:
			self.write(ERR_SMS_FAILED)
			return
		log.debug('SMS:'+res)
		r = re.compile('(\d+),(\d+)')
		s = r.search(res)
		if s.group(2)=='0':
			self.write(ERR_SUCCESS)
		else:
			self.write(ERR_SMS_FAILED)
		# headers = {
		# 	'X-LC-Id': 'ea7hF9zG96VLeTyed6KoEs91-gzGzoHsz', 
		# 	'X-LC-Key': '2eLksG66OWVE2RkYL9GNWLFs',
		# 	'Content-Type': 'application/json',
		# }
		# res_str = http.post('https://api.leancloud.cn/1.1/requestSmsCode', headers, '{"mobilePhoneNumber": "'+phone+'"}')
		# res = json.loads(res_str)
		# if len(res)==0:
		# 	log.info('[%s] send sms[%s]', self.current_user, phone)
		# 	self.write(ERR_SUCCESS)
		# else:
		# 	log.err('[%s] send [%s] sms error[%s]', self.current_user, phone, res_str)
		# 	self.write(ERR_SMS_FAILED)

	# @tornado.web.authenticated
	# def post(self, phone):
	# 	captcha = self.get_argument('captcha', None)
	# 	headers = {
	# 		'X-LC-Id': 'ea7hF9zG96VLeTyed6KoEs91-gzGzoHsz', 
	# 		'X-LC-Key': '2eLksG66OWVE2RkYL9GNWLFs',
	# 		'Content-Type': 'application/json',
	# 	}
	# 	res = http.post('https://api.leancloud.cn/1.1/verifySmsCode/%s?mobilePhoneNumber=%s'%(captcha, phone), headers, None)
	# 	self.write(res)

	@staticmethod
	def verifySmsCode(phone, captcha):
		if SMSHandler.captchaMap.has_key(phone) and SMSHandler.captchaMap[phone][0]==int(captcha):
			return True
		else:
			return False
