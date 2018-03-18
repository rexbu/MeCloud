import re
import datetime
import time

class LangUtils:
    regex = ur'^(?:(?!0000)[0-9]{4}(?:(?:0[1-9]|1[0-2])(?:0[1-9]|1[0-9]|2[0-8])|(?:0[13-9]|1[0-2])(?:29|30)|(?:0[13578]|1[02])31)|(?:[0-9]{2}(?:0[48]|[2468][048]|[13579][26])|(?:0[48]|[2468][048]|[13579][26])00)0229)$'
    @staticmethod
    def validateDate(date):
        if date is None:
            return False
        if re.search(LangUtils.regex,date) == None:
            return False
        return time.mktime(time.strptime(date, "%Y%m%d")) < time.time()