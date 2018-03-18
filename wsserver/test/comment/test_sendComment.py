import httplib

cookie = 'u="12345678910"'
base = 'localhost'
# base = 'n01.me-yun.com'
path = '/1.0/comment/send'
body = '{"c":"11111","to_id":"59faaedbca71432f18d32e2f","c_type":0,"m_id":"596cac72b4b33e28c0e746f8"}'

try:
    header = {'Cookie': cookie, 'X-MeCloud-Debug': 1}
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
