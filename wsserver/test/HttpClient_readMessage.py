import httplib
#userid 59dc6266ca714327af2e4e85
cookie = 'u="12345678910"'
# cookie = 'u="2|1:0|10:1508570328|1:u|32:NTlkYzYyNjZjYTcxNDMyN2FmMmU0ZTg1|98788f9a7e0f7e7ca101598cf14301ac7c9d5d05171c61e4a5299ed3c31420b6"'
base = 'localhost'
# base = 'n01.me-yun.com'
path = '/1.0/push/readMessage'

body = '{"message_id":"5a052021ca714321ab50363a","from_id":"5a018adeca714319e603ca09"}'

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
