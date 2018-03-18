# install ws4py
# pip install ws4py
# easy_install ws4py
import threading
import traceback
import uuid

import time
from ws4py.client.threadedclient import WebSocketClient


class WsClient(WebSocketClient):
    def opened(self):
        pass
        # self.send("hi, opened")

    def closed(self, code, reason=None):
        print "Closed down", code, reason

    def received_message(self, m):
        print 'on message:'
        print m


def to_connect(cookie):
    try:
        xcookie = 'u="' + str(cookie) + '"'
        url = 'ws://n01.me-yun.com:8000/ws'
        # url = 'ws://localhost:8000/ws'
        ws = WsClient(url, protocols=['chat'], headers=[('X-Cookie', xcookie), ('X-test', 'x-test-header-value')])
        print 'has run forever pre 2'
        ws.connect()
        print 'has run forever pre 1'
        ws.run_forever()
        print 'has run forever'
    except KeyboardInterrupt:
        ws.close()


if __name__ == '__main__':
    threads = []
    for num in range(0, 2):
        # t1 = threading.Thread(target=to_connect(num))
        uuidnew = uuid.uuid1()
        threads.append(threading.Thread(target=to_connect, args=(uuidnew,)))
    print threads
    try:
        i = 0
        for t in threads:
            t.setDaemon(False)
            t.start()
            i = i + 1
            print str(i) + 'thread start'
            time.sleep(0.1)
            # connect()
    except Exception, e:
        print e
        msg = traceback.format_exc()
        print msg
