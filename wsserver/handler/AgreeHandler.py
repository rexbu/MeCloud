# coding=utf8

import tornado.web
class AgreeHandler(tornado.web.RequestHandler):
	def get(self, action=None):
		self.render("agree.html")