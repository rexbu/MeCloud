import httplib
import json
import traceback

# cookie = 'u="2|1:0|10:1512030030|1:u|32:NWExZTgyOTQxMGZmNjc1ZDZhMjEyZjli|54e64803567e79b03da66ff2b1ca59ed7999739d089f57c6a99f919590889efe"'
cookie = 'u="kkk"'
# cookie = 'u="123"'
base = 'localhost'
# base = 'n01.me-yun.com'
# base = 'api.videer.net'
path = '/1.0/album/list?size=7&type=16&id=5a1e941ec876b97f3d33af80'
# '&id=5a1e8881c876b9378e059b1d'
# body = '{"c":"hello","to_id":"59ca0b46ca714306705996dc","c_type":"0"}'

try:
    header = {'Cookie': cookie, 'X-MeCloud-Debug': 1}
    print header
    httpClient = httplib.HTTPConnection(base, 8000, timeout=30)
    httpClient.request("GET", path, None, header)

    response = httpClient.getresponse()
    print response.status
    print response.reason
    s = response.read()
    print s
    obj = json.loads(s)
    print 'obj:', obj
    print 'count:', obj['list'].__len__()

    # print response.msg
    # print response.getheaders()
except Exception, e:
    print e
    msg = traceback.format_exc()
    print msg
finally:
    if httpClient:
        httpClient.close()
