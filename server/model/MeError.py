# -*- coding: utf-8 -*-
# coding=utf-8
import fileinput
import tornado.escape
import sys
import os


class MeException(Exception):
    def __init__(self, code, msg, info=None):
        Exception.__init__(self, code, msg, info)
        self.message = {}
        self.message['errCode'] = code;
        self.message['errMsg'] = msg;
        self.message['info'] = info;

    def __str__(self):
        return str(self.message)


ERR_SUCCESS = MeException(0, '执行成功', 'sucess')
ERR_INVALID = MeException(1, '服务器内部错误或者参数错误', 'Internal server error. No information available.')
ERR_OBJECTID_MIS = MeException(104, '错误的ID', 'Missing object id.')
ERR_UNIQUE_KEY = MeException(137, '违反唯一性索引约束（unique），尝试存储重复的值。',
                             'A unique field was given a value that is already taken.')
ERR_APP_OFFLINE = MeException(140, '应用下线，欠费或者容量限额等', '')
ERR_USER_MISSING = MeException(200, '用户名不存在或为空', 'Username is missing or empty')
ERR_PASSWD_MISSING = MeException(201, '密码不存在或为空', 'Password is missing or empty.')
ERR_USER_TAKEN = MeException(202, '用户名已经被占用', 'Username has already been taken.')
ERR_USERPWD_MISMATCH = MeException(210, '用户名或密码错误', 'The username and password mismatch.')
ERR_USER_NOTFOUND = MeException(211, '没有该用户', 'Could not find user.')
ERR_PASSWD_INVALID = MeException(218, '无效的密码', 'Invalid password, it must be a non-blank string.')


# 用户钱包相关错误
ERR_ACCOUNT_BALANCE = MeException(221, '账户余额不足', 'Account balance error')
ERR_ACCOUNT_WALLET = MeException(222, '账户扣费请求失败', 'Account set wallet error')
ERR_ACCOUNT_GOODS = MeException(223, '获取商品请求失败,请稍后再试！', 'Account get goods error')
ERR_APPLE_CERTIFICATE = MeException(224, '客户端未上传苹果验证串！', 'Get apple certificate error')

ERR_BLACK_PERMISSION = MeException(230, '用户被拉黑', 'Black user')
ERR_BLACKED_PERMISSION = MeException(231, '已拉黑该用户', 'Blacked user')
ERR_BEBLACKED_PERMISSION = MeException(232, '该用户设置了关注权限，你还不能关注ta哦', 'No permission to follow him')

ERR_DUPLICATE_MEDIA_ASSIGN = MeException(250, '这张照片已经贡献过了呀', 'Duplicate assign error')
ERR_DUPLICATE_FACE_ASSIGN = MeException(380, '这张脸已经贡献给别人了呀，换一个吧', 'Duplicate assign face error')
# 未经授权的访问，没有提供 App id，或者 App id 和 App key 校验失败，请检查配置。
ERR_UNAUTHORIZED = MeException(401, '未经授权的访问', 'Unauthorized')
# 注册用户受限，应用超过最大用户数
ERR_USER_PERMISSION = MeException(402, '受限的用户权限', 'User authority limited')
# class权限
ERR_CLASS_PERMISSION = MeException(403, '未获取的权限', 'Forbidden to xxx by class permissions')

ERR_PATH_PERMISSION = MeException(404, '请求路径错误', 'Path error')

# 用戶cookie鉴权失败,非法访问
ERR_LOGIN_AUTH_PERMISSION = MeException(405, '用戶token鉴权失败,非法访问', 'Error cookie')

# 验证码
ERR_SMS_FREQUENT = MeException(601, '发送短信过于频繁。验证类短信限制一分钟一条，每天每个号码限制在	10条左右', "Can't send SMS too frequently")
ERR_SMS_FAILED = MeException(602, '发送短信验证码失败，短信提供商返回错误，如果确认手机号码没有问题，请联系我们处理', 'Fails to send message')
ERR_SMS_INVALID = MeException(603, '无效的短信验证码，不匹配或者过期', 'Invalid SMS code')
ERR_AUTH_CAPTCHA = MeException(607, '验证码错误', 'captcha code error');

ERR_PARA = MeException(1000, '参数错误', 'param error')
ERR_NOTFOUND = MeException(1001, '对象不存在', 'object not found')
ERR_NOPARA = MeException(1003, '缺少参数', 'limit of param')

ERR_COOKIE = MeException(2000, 'cookie错误', 'cookie error')

# 数据库错误
ERR_DB_CONNECT = MeException(700, '数据库无法连接', 'Missing database connection')
ERR_OBJECT_DUP = ERR_UNIQUE_KEY
ERR_OBJECT_MIS = ERR_OBJECTID_MIS
ERR_TRANSACTION = MeException(706, '事务执行错误', 'Transaction error')
ERR_DB_OPERATION = MeException(707, '数据库错误', 'Database operation error')
ERR_DEVICE_ALREADY_ACTIVE = MeException(801, '设备已激活,无需重复激活', 'device already active')
ERR_QRCODE_TO_LOGIN = MeException(802, '无效的二维码', 'qrcode is error to login')

def success(json):
    return {'result': True, 'param': json}


def failed(json):
    return {'result': False, 'param': json}
