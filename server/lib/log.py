#-*- coding: utf-8 -*- 
from datetime import *
import sys,os

def nowstr():
	#return datetime.strftime(datetime.now(), '%Y-%m-%d %H:%M:%S.%f%Z')
	t = datetime.strftime(datetime.now(), '%m-%d %H:%M:%S.%f')
	return t+("[%d]" % os.getpid())

def debug(format, *args, **kwargs):
	prefix = '[debug]'+nowstr()+': '
	print prefix+(format%args)
	sys.stdout.flush()

def info(format, *args, **kwargs):
	prefix = '[info]'+nowstr()+': '
	print prefix+(format%args)
	sys.stdout.flush()

def warn(format, *args, **kwargs):
	prefix = '[warn]'+nowstr()+': '
	print prefix+(format%args)
	sys.stdout.flush()

def err(format, *args, **kwargs):
	prefix = '[err]'+nowstr()+': '
	print prefix+(format%args)
	sys.stdout.flush()
