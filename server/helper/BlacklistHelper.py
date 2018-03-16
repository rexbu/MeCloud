#coding=utf-8
from mecloud.helper.ClassHelper import ClassHelper
from mecloud.lib import log


class BlacklistHelper:
    blacklistAuth = None
    def __init__(self, classname):
        self.classname = classname
        if not self.blacklistAuth:
            blacklistAuthHelper = ClassHelper("BlacklistAuth")
            items = blacklistAuthHelper.find({})
            if items:
                self.blacklistAuth = items
    def filterAuTH(self, userId, obj):
        for auth in self.blacklistAuth:
            if self.classname == auth['classname']:
                classHelper = ClassHelper("Blacklist")
                item = classHelper.find_one({'user':obj[auth['foreign']], "blacker":userId})
                if item:
                    log.warn("user %s black blacker %s",obj[auth['foreign']],userId)
                    return None
        return obj




