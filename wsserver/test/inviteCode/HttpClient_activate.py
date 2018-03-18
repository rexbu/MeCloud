import httplib
import traceback

cookie = 'u="123456789"'
# base = 'localhost'
base = 'n01.me-yun.com'
path = '/1.0/invitecode/activate'
body = '{"device":"xxx4","code":"3W4M"}'

try:
    header = {'X-MeCloud-Debug': 1}
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
    msg = traceback.format_exc()
    print msg
finally:
    if httpClient:
        httpClient.close()
