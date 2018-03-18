__author__ = 'wei'

from getui.protobuf import *
import igt_base_template

class LinkTemplate(igt_base_template.BaseTemplate):
    def __init__(self):
        igt_base_template.BaseTemplate.__init__(self)
        self.text = ""
        self.title = ""
        self.logo = ""
        self.logoURL = ""
        self.url = ""
        self.isRing = True
        self.isVibrate = True
        self.isClearable = True
        self.pushType = "NotifyMsg"

    def getActionChains(self):
        #set actionchain
        actionChain1 = gt_req_pb2.ActionChain()
        actionChain1.actionId = 1
        actionChain1.type = gt_req_pb2.ActionChain.Goto
        actionChain1.next = 10000

        #start up app
        actionChain2 = gt_req_pb2.ActionChain()
        actionChain2.actionId = 10000
        actionChain2.type = gt_req_pb2.ActionChain.notification
        actionChain2.title = self.title
        actionChain2.text = self.text
        actionChain2.logo = self.logo
        actionChain2.logoURL = self.logoURL
        actionChain2.ring = self.isRing
        actionChain2.clearable = self.isClearable
        actionChain2.buzz = self.isVibrate
        actionChain2.next = 10010

        #goto
        actionChain3 = gt_req_pb2.ActionChain()
        actionChain3.actionId = 10010
        actionChain3.type = gt_req_pb2.ActionChain.Goto
        actionChain3.next = 10030

        #start web
        actionChain4 = gt_req_pb2.ActionChain()
        actionChain4.actionId = 10030
        actionChain4.type = gt_req_pb2.ActionChain.startweb
        actionChain4.url = self.url
        actionChain4.next = 100

        #end
        actionChain5 = gt_req_pb2.ActionChain()
        actionChain5.actionId = 100
        actionChain5.type = gt_req_pb2.ActionChain.eoa

        actionChains = [actionChain1, actionChain2, actionChain3, actionChain4, actionChain5]

        return actionChains
