# install ws4py
# pip install ws4py
# easy_install ws4py
from ws4py.client.threadedclient import WebSocketClient


class WsClient(WebSocketClient):
    def opened(self):
        pass
        self.send('{"t":"msg_inputting", "to_id":"5a018d14ca714319e603ca18"}')
        # self.send('{"t":"comment_inputting", "to_id":"59ca0b46ca714306705996dc","m_id":"xxxxx_m_id"}')

    def closed(self, code, reason=None):
        print "Closed down", code, reason

    def received_message(self, m):
        print 'on message:'
        print m


if __name__ == '__main__':
    try:
        # userid = '59ca0b46ca714306705996dc'
        # 'u="2|1:0|10:1506413382|1:u|32:NTljYTBiNDZjYTcxNDMwNjcwNTk5NmRj|6c5a31e149f297ffdc10190f9bea527ee72be807723725279a4e2104e1d3c580"'
        # url = 'ws://n01.me-yun.com:8000/ws'
        # url = 'ws://localhost:8000/ws'
        url = 'ws://api.videer.net/ws'
        ws = WsClient(url, protocols=['chat'], headers=[('X-Cookie',
                                                         'u="12345678910"'),
                                                        ('X-test', 'x-test-header-value')])
        ws.connect()
        ws.run_forever()
    except KeyboardInterrupt:
        ws.close()
