import httplib

# cookie = 'u="2|1:0|10:1506413382|1:u|32:NTljYTBiNDZjYTcxNDMwNjcwNTk5NmRj|6c5a31e149f297ffdc10190f9bea527ee72be807723725279a4e2104e1d3c580"'
base = 'localhost'
# base = 'n01.me-yun.com'
path = '/1.0/user/signup'
body = '{"username":"123","password":"cc"}'

try:
    # header = {'Cookie': cookie}#, 'X-MeCloud-Debug': 1}
    # print header
    httpClient = httplib.HTTPConnection(base, 8000, timeout=30)
    httpClient.request("POST", path, body, {})

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
