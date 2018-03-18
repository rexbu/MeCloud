import httplib
import traceback

# cookie = 'u="2|1:0|10:1512030030|1:u|32:NWExZTgyOTQxMGZmNjc1ZDZhMjEyZjli|54e64803567e79b03da66ff2b1ca59ed7999739d089f57c6a99f919590889efe"'
# cookie = 'u="123"'
cookie = 'u="kkk"'
base = 'localhost'
# base = 'api.videer.net'
# base = 'n01.me-yun.com'
path = '/1.0/album/index'
# body = '{"c":"hello","to_id":"59ca0b46ca714306705996dc","c_type":"0"}'

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
