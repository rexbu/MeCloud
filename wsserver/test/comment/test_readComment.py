import httplib

# cookie = 'u="2|1:0|10:1506671668|1:u|32:NTljZGZhZDU1MTE1OWEyODRlZWViYzQw|b73bf9a0c49bc6e2213a58b12dec2e0d94fc0d42f1ff9f30dfa51335e2a00a77"'
cookie = 'u="tttttt"'
base = 'localhost'
# base = 'n01.me-yun.com'
path = '/1.0/comment/read'

body = '{"from_id":"5a094700ca71433ad969500d","message_ids":"5a13d3d9ca71431827f3bd70","m_id":"5a0ebc218b0ed0fddbc8aa1a"}'

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
