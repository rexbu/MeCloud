# coding=utf8
import httplib

cookie = 'u="12345678910"'
base = 'localhost'
# base = 'n01.me-yun.com'
path = '/1.0/thirdpush/push'
# 5a0457b3ca71437840e6b86e linlin
# 5a0a7f88ca714378820c022f tttttt
body = '{"userid":"5a0457b3ca71437840e6b86e","action":"similarFace","otherid":"111"}'
#haohao 59f82f62ca71432ef296198a
#hx 59ef0b09ca71437ccf7d7fcb
try:
    header = {'X-MeCloud-Debug': 1}  # 'Cookie': cookie,
    print header
    httpClient = httplib.HTTPConnection(base, 8000, timeout=30)
    httpClient.request("POST", path, body, header)

    response = httpClient.getresponse()
    print response.status
    print response.reason
    print response.read()
    # print response.msg
    # print response.getheaders()
except Exception, e:
    print e
finally:
    if httpClient:
        httpClient.close()
