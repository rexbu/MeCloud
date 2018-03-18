import httplib

# cookie = 'u="2|1:0|10:1506671668|1:u|32:NTljZGZhZDU1MTE1OWEyODRlZWViYzQw|b73bf9a0c49bc6e2213a58b12dec2e0d94fc0d42f1ff9f30dfa51335e2a00a77"'
cookie='u="2|1:0|10:1508988750|1:u|32:NTlkZjQyNmJjYTcxNDMwNTNiNGUxNTk5|1dd0a85f809ede4a8889b58869af5eae42917e22665332e01fad9511cb6bdacc"'
# base = 'localhost'
base = 'n01.me-yun.com'
path = '/1.0/thirdpay/createWxAppOrder'
body = '{"id":"59e86f5b28575136b08422b1"}'

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
