# -*- coding: utf-8 -*-
'''
 * file :	MeFile.py
 * author :	bushaofeng
 * create :	2016-06-15 19:27
 * func : 
 * history:
'''
import json
from mecloud.model.SmsCode import SmsCodeConfig
from mecloud.model.MeObject import MeObject
import oss2
import time
import datetime
import base64
import hmac
from hashlib import sha1 as sha
import urllib
import urllib2
import os
from aliyunsdkcore import client
from aliyunsdksts.request.v20150401 import AssumeRoleRequest

class MeFileConfig():
    access_key_id = None
    access_key_secret = None
    bucket_name = None
    platform = None
    endpoint = None
    sts_role_arn = None
    role_session_name = None
    region_id = None
    auth = None
    bucket = None
    bucketUrl = None
    # 过期时间
    expire_time = 600
    # 最大文件大小
    max_size = 10000000



class MeFile(MeObject):
    def __init__(self, obj=None):
        MeObject.__init__(self, 'File', obj)

    def setName(self, name):
        if name != None:
            self['name'] = name
            namesplit = name.split('.')
            if len(namesplit) > 1:
                self['type'] = namesplit[len(namesplit) - 1]

    ### 根据id建一个空的对象
    @staticmethod
    def createWithId(oid):
        file = MeFile()
        file.setOverLoad('_id', oid)
        file.objectId = oid
        return file

    def get_iso_8601(self, expire):
        gmt = datetime.datetime.fromtimestamp(expire).isoformat()
        gmt += 'Z'
        return gmt

    def postPara(self):
        now = int(time.time())
        expire_syncpoint = now + MeFileConfig.expire_time
        expire = self.get_iso_8601(expire_syncpoint)
        policy_dict = {}
        policy_dict['expiration'] = expire
        condition_array = []
        array_item = []
        array_item.append('content-length-range');
        array_item.append(1);
        array_item.append(MeFileConfig.max_size);
        condition_array.append(array_item)
        # condition_array.append({'bucket':'vsfile'})
        policy_dict['conditions'] = condition_array
        policy = json.dumps(policy_dict).strip()
        policy_encode = base64.b64encode(policy)
        h = hmac.new('NRMFbx4qByQB43iojOqM6XsbCuvtjn', policy_encode, sha)
        signature = base64.encodestring(h.digest()).strip()
        # type = get_argument('filename').split('.')[-1]
        return {
            'policy': policy_encode,
            'signature': signature,
            'fileName': self.objectId + '.' + self['type'],
            'id': self.objectId,
            'access': 'CYZ1M9y8GMVaC1So',
            'bucketUrl': MeFileConfig.bucketUrl,
        }

    def getUrl(self):
        total = 0
        filename = self.objectId
        if self.has_key('type'):
            filename += ('.' + self['type'])
        # 刚刚上传到oss的文件可能查询不到，需要等一会
        while (not MeFileConfig.bucket.object_exists(filename)):
            time.sleep(1)
            total += 1
            # 最多等待3秒
            if total >= 3:
                return None
        return MeFileConfig.bucket.sign_url('GET', filename, 300)

    def getOSSFileName(self):
        filename = self.objectId
        if self.has_key('type'):
            filename += ('.' + self['type'])
        return filename

    ### 上传
    def upload(self):
        if self.dirty and not self.objectId == None:
            self.dirty['bucketName'] = MeFileConfig.bucket_name
            self.dirty['regionId'] = MeFileConfig.region_id
            self.dirty['platform'] = MeFileConfig.platform
        self.save()

    # 以下代码展示了STS的用法，包括角色扮演获取临时用户的密钥、使用临时用户的密钥访问OSS。

    # STS入门教程请参看  https://yq.aliyun.com/articles/57895
    # STS的官方文档请参看  https://help.aliyun.com/document_detail/28627.html

    # 首先初始化AccessKeyId、AccessKeySecret、Endpoint等信息。
    # 通过环境变量获取，或者把诸如“<你的AccessKeyId>”替换成真实的AccessKeyId等。
    # 注意：AccessKeyId、AccessKeySecret为子用户的密钥。
    # RoleArn可以在控制台的“访问控制  > 角色管理  > 管理  > 基本信息  > Arn”上查看。
    #
    # 以杭州区域为例，Endpoint可以是：
    #   http://oss-cn-hangzhou.aliyuncs.com
    #   https://oss-cn-hangzhou.aliyuncs.com
    # 分别以HTTP、HTTPS协议访问。

    def fetch_sts_token(self):
        """子用户角色扮演获取临时用户的密钥
        :param access_key_id: 子用户的 access key id
        :param access_key_secret: 子用户的 access key secret
        :param role_arn: STS角色的Arn
        :return StsToken: 临时用户密钥
        """
        clt = client.AcsClient(MeFileConfig.access_key_id, MeFileConfig.access_key_secret, MeFileConfig.region_id)
        req = AssumeRoleRequest.AssumeRoleRequest()

        req.set_accept_format('json')
        req.set_RoleArn(MeFileConfig.sts_role_arn)
        req.set_RoleSessionName(MeFileConfig.role_session_name)

        body = clt.do_action_with_exception(req)
        print body
        j = json.loads(body)

        token = {
            'access_key_id': j['Credentials']['AccessKeyId'],
            'access_key_secret': j['Credentials']['AccessKeySecret'],
            'expiration': oss2.utils.to_unixtime(j['Credentials']['Expiration'], '%Y-%m-%dT%H:%M:%SZ'),
            'security_token': j['Credentials']['SecurityToken'],
            'bucket': MeFileConfig.bucket_name,
            'region_id': "oss-"+MeFileConfig.region_id,
            "platform":MeFileConfig.platform,
            "system_time":int(time.time()*1000)
        }
        return token

    @staticmethod
    def upload_data(file_name, data):
        auth = oss2.Auth(SmsCodeConfig.access_key_id, SmsCodeConfig.access_key_secret)
        bucket = oss2.Bucket(auth, MeFileConfig.endpoint, MeFileConfig.bucket_name)
        bucket.put_object(file_name, data)

    

    def download_data(self, process=None):
        auth = oss2.Auth(SmsCodeConfig.access_key_id, SmsCodeConfig.access_key_secret)
        bucket = oss2.Bucket(auth, "oss-" + MeFileConfig.region_id + ".aliyuncs.com", self.__getitem__('bucket'))
        if process==None:
            result = bucket.get_object(self.__getitem__("name"), process=process)
        else:
            result = bucket.get_object(self.__getitem__("name"))
        
        data = result.read()
        return data