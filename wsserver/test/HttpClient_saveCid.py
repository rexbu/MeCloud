import httplib

cookie = 'u="12345678910"'
base = 'localhost'
# base = 'n01.me-yun.com'
path = '/1.0/userpush/saveCid'
body = '{"platform":0,"cid":"897b3b414baf307c06301b2383220c93"}'

try:
    header = {'Cookie': cookie, 'X-MeCloud-Debug': 1}  # 'Cookie': cookie,
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
finally:
    if httpClient:
        httpClient.close()
