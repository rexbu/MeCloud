from mecloud.helper.RedisHelper import RedisDb


def get_current_user(self):
    # userid = self.get_secure_cookie("u")
    # if userid is None:
    #     userid = self.get_userid_from_cookie(str(self.get_cookie("u")))
    self.userid = None
    try:
        cookie_in_header = self.request.headers['Cookie'].split('"')[1]
        print 'cookie_in_header:', cookie_in_header
        if cookie_in_header:
            redis_userid = self.get_userid_from_cookie(cookie_in_header)
            print 'redis_userid:', redis_userid
            if redis_userid:
                redis_cookie = RedisDb.get(redis_userid)
                print 'redis_cookie:', redis_cookie
                if redis_cookie and redis_cookie == cookie_in_header:
                    self.userid = redis_userid
                    print 'find userid:', self.userid
    except Exception, e:
        pass
        print e
    if not self.userid:
        print 'error cookie'
        # self.write(ERR_LOGIN_AUTH_PERMISSION.message)
        # self.finish()
        return None
    else:
        return self.userid