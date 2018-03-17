# coding=utf8
import traceback

import tornado.web
from mecloud.api.BaseHandler import BaseHandler, ERR_PARA, MeObject, ERR_SUCCESS, deepcopy
from mecloud.lib import InviteCodeUtil


class CmsInviteCodeHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self, action=None):
        try:
            if action == 'createInviteCode':
                self.createInviteCode()
            else:
                print 'action error ', action
        except Exception, e:
            print e
            msg = traceback.format_exc()
            print msg
            self.write(ERR_PARA.message)

    def post(self, action=None):
        pass

    def createInviteCode(self):
        count = int(self.get_argument('count', 1))
        if count > 100:
            count = 100
        i = 0
        list = []
        while (True):
            try:
                invite_code = MeObject('InviteCode')
                invite_code['code'] = InviteCodeUtil.create_code()
                invite_code['status'] = 0
                invite_code.save()
                list.append(invite_code['code'])
                i = i + 1
                print 'create invite code finish ', i
                if i >= count:
                    break
            except Exception, e:
                print e
        print 'all finish'
        result = deepcopy(ERR_SUCCESS.message)
        result['list'] = list
        self.write(result)
