import httplib
import traceback

cookie = 'u="2|1:0|10:1506671668|1:u|32:NTljZGZhZDU1MTE1OWEyODRlZWViYzQw|b73bf9a0c49bc6e2213a58b12dec2e0d94fc0d42f1ff9f30dfa51335e2a00a77"'
# cookie = 'u="2|1:0|10:1509600987|1:u|32:NTlmYWFlZGJjYTcxNDMyZjE4ZDMyZTJm|902a21aa1328c12040add1099e94d20034cff5f57ce92e730821331c427bbcc4"'
# cookie = 'u="2|1:0|10:1509541216|1:u|32:NTlkYzU4YTljYTcxNDMyNThiY2Y2YmM3|5c12212e8f633ec69705e2fa80dd075ba444297358f93f9a0b66a55cf59c761e"'
# base = 'localhost'
base = 't01.me-yun.com'
# path = '/1.0/class/Followee'
path = '/1.0/class/CoinSetting?where={"os":0,"platform":0,"status":1}&keys={"os":0,"appleId":0,"status":0,"image":0}&sort={"price":1}'
#Followee
#CoinSetting?where={"os":0,"platform":0,"status":1}&keys={"os":0,"appleId":0,"status":0,"image":0}&sort={"price":1}
body = '{"c":"hello","to_id":"59ca0b46ca714306705996dc","c_type":"0"}'

try:
    header = {'Cookie': cookie, 'X-MeCloud-Debug': 1}
    print header
    httpClient = httplib.HTTPConnection(base, 8000, timeout=30)
    httpClient.request("GET", path, body, header)

    response = httpClient.getresponse()
    print response.status
    print response.reason
    print response.read()
    # print response.msg
    # print response.getheaders()
except Exception, e:
    print e
    msg = traceback.format_exc()
    print msg
finally:
    if httpClient:
        httpClient.close()
