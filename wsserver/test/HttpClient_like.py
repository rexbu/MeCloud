# coding:utf-8
import httplib

cookie = 'u="2|1:0|10:1506671668|1:u|32:NTljZGZhZDU1MTE1OWEyODRlZWViYzQw|b73bf9a0c49bc6e2213a58b12dec2e0d94fc0d42f1ff9f30dfa51335e2a00a77"'
base = 'localhost'
# base = 't01.me-yun.com'
#this file id make crash 59e08fa431a08aa100f5c135
path = '/search/like/2?file=59e0a28b31a08a6990ce3998'
body = '{"c":"hello","to_id":"59faaedbca71432f18d32e2f","c_type":0}'

# test 59faaedbca71432f18d32e2f
# 王勇 59f2aa4fca714375128dc5df
# 国东 59f00a36ca71431fa5fb64a7
# 百龙　59dc6266ca714327af2e4e85
# gs 59ef21c1ca71437ccfdee3fd
try:
    header = {'Cookie': cookie, 'X-MeCloud-Debug': 1}
    print header
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
