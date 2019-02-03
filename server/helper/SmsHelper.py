# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

import sys, uuid
from aliyunsdkcore.request import RpcRequest
from aliyunsdkcore.client import AcsClient
from aliyunsdkcore.request import CommonRequest

class SmsCodeConfig():
    region = None
    access_key_id = None
    access_key_secret = None
    template_code = None
    sign_name = None

class SmsCode:
	@staticmethod
	def sendSms(phone, code):
		client = AcsClient(SmsCodeConfig.access_key_id, SmsCodeConfig.access_key_secret, 'default')

		request = CommonRequest()
		request.set_accept_format('json')
		request.set_domain('dysmsapi.aliyuncs.com')
		request.set_method('POST')
		request.set_protocol_type('https') # https | http
		request.set_version('2017-05-25')
		request.set_action_name('SendSms')

		request.add_query_param('PhoneNumbers', str(phone))
		request.add_query_param('SignName', SmsCodeConfig.sign_name)
		request.add_query_param('TemplateCode', SmsCodeConfig.template_code)
		request.add_query_param('TemplateParam', '{"code":"%s"}'%(str(code)))

		response = client.do_action(request)
		# python2:  print(response) 
		print(str(response))
		return response