import httplib
import traceback

cookie = 'u="2|1:0|10:1512389300|1:u|32:NWExYTU4NGRjYTcxNDM3Y2E0YzAxM2Zh|430aba92bbd921afccacaf4b5af8fe09cc3e0aca09b15beaab444fea7bb8f464"'
# cookie = 'u="12345678910"'
base = 'localhost'
# base = 'api.videer.net'
# base = 'n01.me-yun.com'
path = '/1.0/comment/unreadList?all=1'
body = '{"c":"hello","to_id":"59ca0b46ca714306705996dc","c_type":"0"}'

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
