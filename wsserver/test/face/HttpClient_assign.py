import httplib

cookie = 'u="2|1:0|10:1506671668|1:u|32:NTljZGZhZDU1MTE1OWEyODRlZWViYzQw|b73bf9a0c49bc6e2213a58b12dec2e0d94fc0d42f1ff9f30dfa51335e2a00a77"'
# base = 'localhost'
base = 'n01.me-yun.com'
path = '/assign/59f2cc95ca714378fddb52f6?media=59faf6a260a0efeb0b2b6f35&face=0&image_id=1509619141.49&price=3'
body = '{"username":"123","password":"cc"}'

try:
    header = {'Cookie': cookie, 'X-MeCloud-Debug': 1}
    httpClient = httplib.HTTPConnection(base, 9000, timeout=30)
    httpClient.request("POST", path, body, header)

    response = httpClient.getresponse()
    print response.status
    print response.reason
    print response.read()
    # print response.msg
    # print response.getheaders()
except Exception, e:
    print e
finally:
    if httpClient:
        httpClient.close()
