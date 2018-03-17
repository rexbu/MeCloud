#-*- coding: utf-8 -*- 
'''
 * file :	ErrorMap.py
 * author :	bushaofeng
 * create :	2016-06-15 22:05
 * func : 
 * history:
'''
import MeError

errors = dir(MeError)
errmap = {}
for e in errors:
	if e.startswith('ERR'):
		err = getattr(MeError, e)
		errmap[err['errCode']] = err

e = Exception(111, 'aaa', 'bbb');
print dir(e)
print e[1]
print e.args
print e.message