# -*- coding: utf-8 -*-
# pip install websocket-client
import traceback
from socket import *
import json, time, threading
from websocket import create_connection
import resource

# config = {
#     'HOST': '127.0.0.1',
#     'PORT': 10086
# }
# pip install websocket-client
from collections import OrderedDict


class Client():
    def __init__(self):
        # 调用create_connection方法，建立一个websocket链接
        # 链接地址请修改成你自己需要的
        try:
            # url ='ws://n01.me-yun.com:8000/ws'
            url = 'ws://localhost:8000/ws'
            self.ws = create_connection(url)
            # 建一个线程，监听服务器发送给客户端的数据
            # threading.stack_size(1024)
            self.trecv = threading.Thread(target=self.recv)
            self.trecv.start()
        except Exception, e:
            print e
            msg = traceback.format_exc()
            print msg

    # 发送方法，聊天输入语句时调用，此处默认为群聊ALL
    def send(self, content):
        # 这里定义的消息体要换成你自己的消息体，变成你需要的。
        msg = {
            "type": "POST",
            "username": "hehe",
            "sendto": "ALL",
            "content": content

        }
        msg = json.dumps(msg)
        self.ws.send(msg)

    # 接收服务端发送给客户的数据，只要ws处于连接状态，则一直接收数据
    def recv(self):
        try:
            while self.ws.connected:
                result = self.ws.recv()
                print "received msg:" + str(result)
        except Exception, e:
            pass

    # 关闭时，发送QUIT方法，退出ws链接
    def close(self):
        # 具体要知道你自己退出链接的消息体是什么，如果没有，可以不写这个方法
        msg = {
            "type": "QUIT",
            "username": "johanna",
            "content": "byebye,everyone"
        }
        msg = json.dumps(msg)
        self.ws.send(msg)


if __name__ == '__main__':
    # print resource.

    for num in range(0, 10):
        try:
            Client()
            print str(num) + ' thread start'

        except Exception, e:
            print e
            msg = traceback.format_exc()
            print msg

            # 当输入非exit时，则持续ws链接状态，d 如果exit，则关闭链接
            # while True:
            #     content = raw_input("please input(input exit to exit):")
            #     if content == "exit":
            #         c.close()
            #         break
            #     else:
            #         c.send(content)
            #         time.sleep(1)
