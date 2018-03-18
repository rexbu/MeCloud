import httplib
import traceback

from mecloud.lib import crypto

cookie = 'u="2|1:0|10:1511932450|1:u|32:NWExNjYxZDcxMGZmNjczOTM5ODM4NGJh|c94e3120cf91a3280072aff03bc9d2391e29f2e3957b8db389ba7a9a4979b764"'
# cookie = 'u="12345678910"'
base = 'localhost'
# base = 'api.videer.net'
# base = 'n01.me-yun.com'
path = '/1.0/push/unreadList'
# body = '{"c":"hello","to_id":"59ca0b46ca714306705996dc","c_type":"0"}'
body = None

try:
    header = {'Cookie': cookie, 'X-MeCloud-Debug': 1}
    print header
    httpClient = httplib.HTTPConnection(base, 8000, timeout=30)
    httpClient.request("GET", path, body, header)

    response = httpClient.getresponse()
    print response.status
    print response.reason
    r = response.read()
    print r
    print crypto.decrypt(r)
    # print response.msg
    # print response.getheaders()
except Exception, e:
    print e
    msg = traceback.format_exc()
    print msg
finally:
    if httpClient:
        httpClient.close()
