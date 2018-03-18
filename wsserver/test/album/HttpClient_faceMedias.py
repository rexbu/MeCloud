import httplib
import traceback

#cookie = 'u="2|1:0|10:1511865930|1:u|32:NWExN2YwNjMxMGZmNjc0NDNjOTA4NmY5|7a7e529b4924b003c1f21924da05d7f14e840dbd2eda903378a3bb7b16146db7"'
# cookie = 'u="2|1:0|10:1511948948|1:u|32:NWExZTgyOTQxMGZmNjc1ZDZhMjEyZjli|841a15ece0292e47023d192badca0cf69d9bb12b894306705ac05ee5c2adba77"'
# cookie = 'u="1234567910"'
cookie = 'u="kkk"'
base = 'localhost'
# base = 'api.videer.net'
# base = 'n01.me-yun.com'
path = '/1.0/album/faceMedias?size=20&face=0'
# body = '{"c":"hello","to_id":"59ca0b46ca714306705996dc","c_type":"0"}'

try:
    header = {'Cookie': cookie, 'X-MeCloud-Debug': 1}
    print header
    httpClient = httplib.HTTPConnection(base, 8000, timeout=30)
    httpClient.request("GET", path, None, header)

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
