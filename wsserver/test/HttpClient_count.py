import httplib
import traceback

cookie = 'u="12345678910"'
# cookie = 'u="2|1:0|10:1509600987|1:u|32:NTlmYWFlZGJjYTcxNDMyZjE4ZDMyZTJm|902a21aa1328c12040add1099e94d20034cff5f57ce92e730821331c427bbcc4"'
base = 'localhost'
# base = 't01.me-yun.com'
path = '/1.0/count/index'
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
    print response.getheaders()
except Exception, e:
    print e
    msg = traceback.format_exc()
    print msg
finally:
    if httpClient:
        httpClient.close()
