import httplib

# cookie = 'u="2|1:0|10:1506413382|1:u|32:NTljYTBiNDZjYTcxNDMwNjcwNTk5NmRj|6c5a31e149f297ffdc10190f9bea527ee72be807723725279a4e2104e1d3c580"'
base = 'localhost'
# base = 'api.videer.net'
# base = 'n01.me-yun.com'
path = '/1.0/user/login'
body = '{"username":"5a01889031a08abb2fa7716a","password":"2dab166b70402fd0daac180542a4bb18"}'
# body = '{"username":"5a01889031a08abb2fa7716a","password":"94a1a6060d2c075965e35da82ed1cb40"}'
# body = '{"username":"B1198666E0564C8587FDC166F2269765","password":"49855c519438269efad8c4c8a874fc93"}'
try:
    header = {'X-MeCloud-Platform': 'iOS', 'X-MeCloud-AppVersion': 1,'X-MeCloud-Debug': 1}
    # print header
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

# 2|1:0|10:1506671668|1:u|32:NTljZGZhZDU1MTE1OWEyODRlZWViYzQw|b73bf9a0c49bc6e2213a58b12dec2e0d94fc0d42f1ff9f30dfa51335e2a00a77
# 59cdfad551159a284eeebc40