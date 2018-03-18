import httplib
import traceback

cookie = 'u="tttttt"'
base = 'localhost'
# base = 'n01.me-yun.com'
path = '/1.0/comment/listByFromId?from_id=5a094700ca71433ad969500d&m_id=5a0ebc218b0ed0fddbc8aa1a&count=100'  # &message_id=5a0bb307ca71432ad3f328d1
body = '{"c":"hello","from_id":"59e8dde0ca71430e0127783b","c_type":"0"}'

try:
    header = {'Cookie': cookie, 'X-MeCloud-Debug': 1}
    print header
    httpClient = httplib.HTTPConnection(base, 8000, timeout=30)
    httpClient.request("GET", path, None, header)

    response = httpClient.getresponse()
    print response.status
    print response.reason
    print response.read()
    # print response.msg
    # print response.getheaders()
except Exception, e:
    print e
    msg = traceback.format_exc()
    print msg
finally:
    if httpClient:
        httpClient.close()
