#-*- coding: utf-8 -*- 
import urllib
import urllib2

def post(url, headers, data):
	req = urllib2.Request(url)
	for header in headers:
		req.add_header(header, headers[header])
	if data!=None:
		print url
		print data
		return urllib2.urlopen(req, data).read()
	else:
		return urllib2.urlopen(req).read()

def get(url):
	pass