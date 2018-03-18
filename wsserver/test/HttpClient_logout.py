import httplib
import traceback

cookie = 'u="123456789"'
base = 'localhost'
# base = 't01.me-yun.com'
path = '/1.0/logout'
body = '{"id":"59f2de31ca71437b4ee9edbc"}'

try:
    header = {'Cookie': cookie, 'X-MeCloud-Debug': 1}
    print header
    httpClient = httplib.HTTPConnection(base, 8000, timeout=30)
    httpClient.request("POST", path, None, header)

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
