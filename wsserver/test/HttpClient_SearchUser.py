# coding=utf8
import httplib
# cookie='u="2|1:0|10:1508988750|1:u|32:NTlkZjQyNmJjYTcxNDMwNTNiNGUxNTk5|1dd0a85f809ede4a8889b58869af5eae42917e22665332e01fad9511cb6bdacc"'
cookie = 'u="12345678910"'
base = 'localhost'
# base = 'n01.me-yun.com'
path = '/1.0/search/user'
# body = '{"content":"gs"}'
scroll_id = 'DnF1ZXJ5VGhlbkZldGNoBQAAAAAAAAAVFmZ1cDdZaXFTUkhtZS1kc0pEOXdId1EAAAAAAAAAFhZmdXA3WWlxU1JIbWUtZHNKRDl3SHdRAAAAAAAAABcWZnVwN1lpcVNSSG1lLWRzSkQ5d0h3UQAAAAAAAAAYFmZ1cDdZaXFTUkhtZS1kc0pEOXdId1EAAAAAAAAAGRZmdXA3WWlxU1JIbWUtZHNKRDl3SHdR'
body = '{"content":"rag", "pageSize":20}'

try:
    header = {'Cookie': cookie, 'X-MeCloud-Debug': 1, 'X-MeCloud-platform':'ios'}
    print header
    httpClient = httplib.HTTPConnection(base, 8000, timeout=30)
    httpClient.request("POST", path, body, header)

    response = httpClient.getresponse()
    print response.status
    print response.reason
    print response.read()
    print response.getheaders()
except Exception, e:
    print e
finally:
    if httpClient:
        httpClient.close()
