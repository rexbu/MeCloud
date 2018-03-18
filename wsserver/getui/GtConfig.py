__author__ = 'Administrator'
import os


class GtConfig:
    def __init__(self):
        pass

    @staticmethod
    def isPushSingleBatchAsync():
        return os.getenv("gexin_pushSingleBatch_needAsync", False)

    @staticmethod
    def isPushListAsync():
        return os.getenv("gexin_pushList_needAsync", False)

    @staticmethod
    def isPushListNeedDetails():
        return GtConfig.getProperty("gexin_pushList_needDetails", "needDetails", False)

    @staticmethod
    def getHttpProxyIp():
        return os.getenv("gexin_http_proxy_ip", None)

    @staticmethod
    def getHttpProxyPort():
        return os.getenv("gexin_http_proxy_port", 80)

    @staticmethod
    def getSyncListLimit():
        return os.getenv("gexin_pushList_syncLimit", 1000)

    @staticmethod
    def getAsyncListLimit():
        return os.getenv("gexin_pushList_asyncLimit", 10000)

    @staticmethod
    def getHttpConnectionTimeOut():
        return os.getenv("gexin_http_connection_timeout", 60)

    @staticmethod
    def getHttpSoTimeOut():
        return os.getenv("gexin_http_so_timeout", 30)

    @staticmethod
    def getHttpTryCount():
        return os.getenv("gexin_http_tryCount", 3)

    @staticmethod
    def getHttpInspectInterval():
        return os.getenv("gexin_http_inspect_interval", 60)

    @staticmethod
    def getDefaultDomainUrl(useSSL):
        hosts = list()
        host = os.getenv("gexin_default_domainurl", None)
        if host is None or "" == host.strip():
            if useSSL :
                hosts.append("https://cncapi.getui.com/serviceex")
                hosts.append("https://telapi.getui.com/serviceex")
                hosts.append("https://api.getui.com/serviceex")
                hosts.append("https://sdk1api.getui.com/serviceex")
                hosts.append("https://sdk2api.getui.com/serviceex")
                hosts.append("https://sdk3api.getui.com/serviceex")
            else:
                hosts.append("http://sdk.open.api.igexin.com/serviceex")
                hosts.append("http://sdk.open.api.gepush.com/serviceex")
                hosts.append("http://sdk.open.api.getui.net/serviceex")
                hosts.append("http://sdk1.open.api.igexin.com/serviceex")
                hosts.append("http://sdk2.open.api.igexin.com/serviceex")
                hosts.append("http://sdk3.open.api.igexin.com/serviceex")
        else:
            for h in host.split(','):
                if h.startswith("https://") and not useSSL:
                    continue
                if h.startswith("http://") and useSSL:
                    continue
                if not h.startswith("http") and useSSL:
                    h = "https://" + h
                hosts.append(h)

        return hosts

    @staticmethod
    def getSDKVersion():
        return "4.0.1.0"

    @staticmethod
    def getProperty(oldKey, newKey, defaultValue):
        newValue = os.getenv(newKey)
        oldValue = os.getenv(oldKey)

        if newValue is not None:
            return newValue
        elif oldValue is not None:
            return oldValue
        else:
            return defaultValue




