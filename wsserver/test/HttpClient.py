# coding:utf-8
import httplib

cookie = 'u="12345678910"'
base = 'localhost'
# base = 't01.me-yun.com'
path = '/1.0/push/sendMessage'
body = '{"c":"honey://emoji/5","to_id":"5a1aea98ca714333dda69c5c","c_type":0}'

# test 59faaedbca71432f18d32e2f
# 王勇 59f2aa4fca714375128dc5df
# 国东 59f00a36ca71431fa5fb64a7
# 百龙　59dc6266ca714327af2e4e85
# gs 59ef21c1ca71437ccfdee3fd
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
