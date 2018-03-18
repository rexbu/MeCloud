__author__ = 'Kevin'

from protobuf import *
import igt_base_template

class NotyPopLoadTemplate(igt_base_template.BaseTemplate):
    def __init__(self):
        igt_base_template.BaseTemplate.__init__(self)
        self.notyIcon = ""
        self.logoUrl = ""
        self.notyTitle = ""
        self.notyContent = ""
        self.isRing = True
        self.isVibrate = True
        self.isClearable = True
        self.popTitle = ""
        self.popContent = ""
        self.popImage = ""
        self.popButton1 = ""
        self.popButton2 = ""
        self.loadIcon = ""
        self.loadTitle = ""
        self.loadUrl = ""
        self.transmissionType = 0
        self.transmissionContent = ""
        self.isAutoInstall = False
        self.isActive = False
        self.pushType = "NotyPopLoadMsg"
        self.androidMark = ""
        self.symbianMark = ""
        self.iosMark = ""

    def getActionChains(self):
        #set actionChain
        actionChain1 = gt_req_pb2.ActionChain()
        actionChain1.actionId = 1
        actionChain1.type = gt_req_pb2.ActionChain.Goto
        actionChain1.next = 10000

        #notification
        actionChain2 = gt_req_pb2.ActionChain()
        actionChain2.actionId = 10000
        actionChain2.type = gt_req_pb2.ActionChain.notification
        actionChain2.title = self.notyTitle
        actionChain2.text = self.notyContent
        actionChain2.logo = self.notyIcon
        actionChain2.logoURL = self.logoUrl
        actionChain2.ring = self.isRing
        actionChain2.clearable = self.isClearable
        actionChain2.buzz = self.isVibrate
        actionChain2.next = 10010

        actionChain3 = gt_req_pb2.ActionChain()
        actionChain3.actionId = 10010
        actionChain3.type = gt_req_pb2.ActionChain.Goto
        actionChain3.next = 10020

        actionChain4 = gt_req_pb2.ActionChain()
        button1 = actionChain4.buttons.add()
        button1.text = self.popButton1
        button1.next = 10040
        button2 = actionChain4.buttons.add()
        button2.text = self.popButton2
        button2.next = 100

        actionChain4.actionId = 10020
        actionChain4.type = gt_req_pb2.ActionChain.popup
        actionChain4.title = self.popTitle
        actionChain4.text = self.popContent
        actionChain4.img = self.popImage
        actionChain4.next = 6

        appStartUp = gt_req_pb2.AppStartUp()
        appStartUp.android = self.androidMark
        appStartUp.symbia = self.symbianMark
        appStartUp.ios = self.iosMark
        actionChain5 = gt_req_pb2.ActionChain()
        actionChain5.actionId = 10040
        actionChain5.type = gt_req_pb2.ActionChain.appdownload
        actionChain5.name = self.loadTitle
        actionChain5.url = self.loadUrl
        actionChain5.logo = self.loadIcon
        actionChain5.autoInstall = self.isAutoInstall
        actionChain5.autostart = self.isActive
        actionChain5.appstartupid.MergeFrom(appStartUp)
        actionChain5.next = 6

        #end
        actionChain6 = gt_req_pb2.ActionChain()
        actionChain6.actionId = 100
        actionChain6.type = gt_req_pb2.ActionChain.eoa

        actionChains = [actionChain1, actionChain2, actionChain3, actionChain4, actionChain5, actionChain6]
        return actionChains
