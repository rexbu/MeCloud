#coding=utf-8
from requests.packages.urllib3.packages.six.moves import urllib

s='%E6%88%91'
# s='中文'
s=urllib.parse.unquote(s)
print(s)


server_only = 0
if not server_only:
    print 'no'
else:
    print 'yes'
