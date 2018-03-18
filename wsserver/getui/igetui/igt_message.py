__author__ = 'wei'

from getui.igetui.template.igt_base_template import *
from getui.igetui.utils.AppConditions import *
class IGtMessage:
    def __init__(self):
        self.isOffline = False
        self.offlineExpireTime = 0
        self.data = BaseTemplate()
        self.pushNetWorkType = 0
        self.priority=0
        
    def isOffline(self):
        return self.isOffline
    
    def setOffline(self,isOffline):
        self.isOffline=isOffline
        
    def getOfflineExpireTime(self):
        return self.offlineExpireTime
    
    def setOfflineExpireTime(self,offlineExpireTime):
        self.offlineExpireTime=offlineExpireTime
    
    def getData(self):
        return self.data
    
    def setData(self,data):
        self.data=data
        
    def getPriority(self):
        return self.priority
    
    def setPriority(self,priority):
        self.priority=priority
        
    def getPushNetWorkType(self):        
        return self.pushNetWorkType
    
    def setPushNetWorkType(self,pushNetWorkType):        
        self.pushNetWorkType=pushNetWorkType
    
    
    
class IGtSingleMessage(IGtMessage) :
    def __init__(self):
        IGtMessage.__init__(self)






class IGtListMessage(IGtMessage):
    def __init__(self):
        IGtMessage.__init__(self)






class IGtAppMessage(IGtMessage):
    def __init__(self):
        IGtMessage.__init__(self)
        self.appIdList = []
        self.phoneTypeList = []
        self.provinceList = []
        self.tagList = []
        self.conditions = None
        self.speed = 0
    def getTagList(self):
        return self.tagList
    
    def setTagList(self,tagList):
        self.tagList=tagList
        
    def getAppIdList(self):
        return self.appIdList
    
    def setAppIdList(self,setAppIdList):
        self.setAppIdList=setAppIdList
        
    def getPhoneTypeList(self):
        return self.phoneTypeList
    
    def setPhoneTypeList(self,phoneTypeList):
        self.phoneTypeList=phoneTypeList
        
    def getProvinceList(self):
        return self.provinceList
    
    def setProvinceList(self,provinceList):
        self.provinceList=provinceList

    def getConditions(self):
        return self.conditions;

    def setConditions(self, conditions):
        self.conditions = conditions

    def getSpeed(self):
        return self.speed
    
    def setSpeed(self,speed):
        self.speed=speed
        
    
    
        
        
















