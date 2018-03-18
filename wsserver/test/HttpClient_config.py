import httplib
import traceback

base = 'localhost'
# base = 'api.videer.net'
# base = 'n01.me-yun.com'
path = '/1.0/config/base'
try:
    header = {'X-MeCloud-Client': '1.0.0.3', 'X-MeCloud-Platform': 'android', 'X-MeCloud-AppVersion':1}
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
