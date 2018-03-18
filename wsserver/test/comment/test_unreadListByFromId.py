import httplib
import traceback

cookie = 'u="2|1:0|10:1506671668|1:u|32:NTljZGZhZDU1MTE1OWEyODRlZWViYzQw|b73bf9a0c49bc6e2213a58b12dec2e0d94fc0d42f1ff9f30dfa51335e2a00a77"'
base = 'localhost'
# base = 'n01.me-yun.com'
path = '/1.0/comment/unreadListByFromId?from_id=59e8dde0ca71430e0127783b&m_id=596cac72b4b33e28c0e746f8'  # ?id=59dc380051159a123fe8a230
body = '{"c":"hello","from_id":"59e8dde0ca71430e0127783b","c_type":"0"}'

try:
    header = {'Cookie': cookie, 'X-MeCloud-Debug': 1}
    print header
    httpClient = httplib.HTTPConnection(base, 8000, timeout=30)
    httpClient.request("GET", path, body, header)

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
