#-*- coding: utf-8 -*-
import urllib
import config
import json
import xmltodict
import time, hashlib
import log

access_token = None
jsapi_ticket = None
def accessToken():
	global access_token;
	# 如果为空或者已经过期，则重新获取
	if access_token==None or (access_token['expires_in']<=time.time()-access_token['createAt']-100):
		access_token = urllib.urlopen('http://%s:%d/wx/access_token?auth=%s' % (config.token['ip'], config.token['port'], config.token['auth'])).read()
	return access_token['access_token'];
def jsapiTicket():
	global jsapi_ticket
	if jsapi_ticket==None or (jsapi_ticket['expires_in']<=time.time()-jsapi_ticket['createAt']-100):
		url = 'http://%s:%d/wx/jsapi_ticket?auth=%s' % (config.token['ip'], config.token['port'], config.token['auth'])
		print url
		jsapi_ticket = urllib.urlopen(url).read()
	return jsapi_ticket

# 获取用户
def getUserInfo(openid):
	token = accessToken()
	url = 'https://api.weixin.qq.com/cgi-bin/user/info?access_token=%s&openid=%s&lang=zh_CN'%(token, openid);
	res = json.loads(urllib.urlopen(url).read())
	if res.has_key('errcode'):
		log.err("Wx get userinfo error: %s, token:%s", json.dumps(res), token)
		return None;
	else:
		return res;
# 获取用户，通过网页授权
def getSnsUserInfo(token, openid):
	url = 'https://api.weixin.qq.com/sns/userinfo?access_token=%s&openid=%s&lang=zh_CN'%(token, openid);
	res = json.loads(urllib.urlopen(url).read())
	if res.has_key('errcode'):
		log.err("Wx get userinfo error: %s", json.dumps(res))
		return None;
	else:
		return res;
def xmltojson(xml):
	return xmltodict.parse(xml)['xml']

RESPONSE_TEXT_TEMPLATE = '<xml>\
<ToUserName><![CDATA[%s]]></ToUserName>\
<FromUserName><![CDATA[%s]]></FromUserName>\
<CreateTime>%u</CreateTime>\
<MsgType><![CDATA[%s]]></MsgType>\
<Content><![CDATA[%s]]></Content></xml>'

def replyText(f, t, c):
	return RESPONSE_TEXT_TEMPLATE%(t, f, (int)(time.time()), 'text', c)
	'''
	o = {
		'xml': {
			'FromUserName': r'<![CDATA['+f+']]>',
			'ToUserName': r'<![CDATA['+t+']]>',
			'CreateTime': (int)(time.time()),
			'MsgType': r'<![CDATA['+u'text'+']]>',
			'Content': r'<![CDATA['+c+']]>',
		},
	}
	print o
	return xmltodict.unparse(o, full_document=False, cdata_key='#FromUserName')
	'''

def accessTokenFromWx():
	global access_token;
	access_string = urllib.urlopen('https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=%s&secret=%s' % (config.wx['appId'], config.wx['appSecret'])).read()
	access_token = json.loads(access_string)
	if access_token.has_key("access_token"):
		access_token['createAt'] = (int)(time.time())
		log.info("fresh access_token: %s", json.dumps(access_token))
		return access_token
	else:
		log.err("AccessToken Error: %s", access_string)
		return None

def accessTokenFromCode(code):
	access_string = urllib.urlopen('https://api.weixin.qq.com/sns/oauth2/access_token?appid=%s&secret=%s&code=%s&grant_type=authorization_code' % (config.wx['appId'], config.wx['appSecret'], code)).read()
	web_token = json.loads(access_string)
	if web_token.has_key("access_token"):
		log.info("sns access_token: %s", access_string)
		return web_token
	else:
		log.err("AccessToken Error: %s", access_string)
		return None

### 从微信服务器获取jsapi_ticket
def jsapiTicketFromWx():
	global jsapi_ticket
	ticket_string = urllib.urlopen('https://api.weixin.qq.com/cgi-bin/ticket/getticket?access_token=%s&type=jsapi'%accessToken()).read()
	jsapi_ticket = json.loads(ticket_string)
	if jsapi_ticket.has_key("ticket"):
		jsapi_ticket['createAt'] = (int)(time.time())
		log.info("fresh jsapi_ticket: %s", ticket_string)
		return jsapi_ticket
	else:
		log.err("JsapiTicket Error: %s", ticket_string)
		return None
### jsapi 签名
def jsapiSignature(noncestr, timestamp, url):
	sig = 'jsapi_ticket=%s&noncestr=%s&timestamp=%d&url=%s'%(jsapiTicket()['ticket'], noncestr, timestamp, url)
	return hashlib.sha1(sig).hexdigest()

def jsapiWxConfig(url):
	wxconfig = {}
	wxconfig['appId'] = config.wx['appId']
	wxconfig['timestamp'] = int(time.time())
	wxconfig['nonceStr'] = 'Wm3WZYTPz0wzCcnW'
	wxconfig['url'] = url
	wxconfig['signature'] = jsapiSignature(wxconfig['nonceStr'], wxconfig['timestamp'], wxconfig['url'])
	return wxconfig
