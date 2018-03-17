# -*- coding: utf-8 -*-
import json
import uuid
from datetime import *
from urllib import *
from mecloud.model.MeObject import *
import isodate


def createOrderNo():
    return str(uuid.uuid4()).replace('-', '')

def date2str(t):
    return datetime.strftime(t, '%Y-%m-%d %H:%M:%S.%f%Z')


def dict2str(obj):
    print obj
    for k in obj.keys():
        if isinstance(obj[k], datetime):
            obj[k] = date2str(obj[k])
    return obj


def urldecode(obj):
    for o in obj:
        obj[o] = unquote(obj[o]).encode('utf-8')
    return obj


### 检查字典里是否有某些参数
def checkKeys(obj, keys):
    if not isinstance(obj, dict):
        return False
    if not (type(keys) is list):
        return False
    for key in keys:
        if not obj.has_key(key):
            return False
    return True


### 检查字典里是否有某些参数并且值是否为空
def checkKeysAndValue(obj, keys):
    if not isinstance(obj, dict):
        return False
    if not (type(keys) is list):
        return False
    for key in keys:
        if not obj.has_key(key):
            return False
        elif len(str(obj[key])) == 0:
            return False
    return True


### 对时间进行编码
class MeEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, datetime):
            # return obj.strftime('%Y-%m-%d %H:%M:%S.%f%Z')
            return isodate.strftime(obj, "%Y-%m-%d %H:%M:%S.%f%Z")
        elif isinstance(obj, date):
            return obj.strftime('%Y-%m-%d')
        # elif isinstance(obj, MeObject):
        # 	return json.dumps(obj, cls=MeEncoder)
        else:
            return json.JSONEncoder.default(self, obj)

def deprecated(func):
    count = [0]
    def wrapper(*args, **kwargs):
        count[0] += 1
        if count[0] == 1:
            print func.__name__, 'is deprecated'
        return func(*args, **kwargs)
    return wrapper