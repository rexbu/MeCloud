# -*- coding: utf-8 -*-
'''
 * file :	crypto.py
 * author :	Rex
 * create :	2017-08-15 13:46
 * func : 
 * history:
'''
from ctypes import *

from os.path import abspath, dirname,os,sys

module = cdll.LoadLibrary(dirname(abspath(__file__))+"/crypto_string.so")


def encrypt(en_str):
    in_str = c_char_p(en_str)
    in_len = c_int(len(en_str))
    # 字符串末尾加\0，所以长度+1
    out_str = create_string_buffer(len(en_str) * 2 + 1)
    out_len = c_int(len(en_str) * 2 + 1)

    size = module.crypto(in_str, in_len, out_str, out_len)
    if size < 0:
        return None
    else:
        return str(out_str.value)


### 解密
def decrypt(de_str):
    in_str = c_char_p(de_str)
    in_len = c_int(len(de_str))
    out_str = create_string_buffer(len(de_str) / 2 + 1)
    out_len = c_int(len(de_str) / 2 + 1)
    size = module.decrypt(in_str, in_len, out_str, out_len)
    if size < 0:
        return None
    else:
        return str(out_str.value)


# 图片加密
def imageEncrypt(en_str):
    imageBytes = bytearray(en_str)
    outByteArray = bytearray()
    j = 0
    for i in range(len(imageBytes)):
        temp = None
        # print imageBytes[i]
        j = j + 1
        if j > 255:
            j = 0

        if imageBytes[i] + (j / 5 + j % 3) > 255:
            temp = imageBytes[i] + (j / 5 + j % 3) - 256
        else:
            temp = imageBytes[i] + (j / 5 + j % 3)
        outByteArray.append(temp)
    return outByteArray

# 图片解密
def imageDecrypt(en_str):
    imageBytes = bytearray(en_str)
    outByteArray = bytearray()
    j = 0
    for i in range(len(imageBytes)):
        temp = None
        j = j + 1
        if j > 255:
            j = 0
        if imageBytes[i] - (j / 5 + j % 3) < 0:
            temp = 256 + imageBytes[i] - (j / 5 + j % 3)
        else:
            temp = imageBytes[i] - (j / 5 + j % 3)
        outByteArray.append(temp)
    return outByteArray
