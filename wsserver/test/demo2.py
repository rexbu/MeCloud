import random


# id1 = '5a08fbfbca714330cfd35ddc'
# id2 = '1a046c8bca71437840e6b872'
# if id2>id1:
#     print 'id2 larger'
#
# if id1>id2:
#     print 'id1 larger'
import string


def GetRandomCode():
    a = random.sample("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", 8)
    code = ""
    code = code.join(a)
    return code


def GetRamdomCodeList(count=50):
    codeList = []
    while len(codeList) < count:
        code = GetRandomCode()
    if code not in codeList:
        codeList.append(code)
    return codeList


# if __name__ == '__main__':
#     # print GetRamdomCodeList(8)
#     salt = ''.join(random.sample(string.ascii_letters + string.digits, 8))
#     print salt
if __name__ == '__main__':
    seed = "23456789ABCDEFGHJKMNPQRSTUVWXYZ"
    sa = []
    for i in range(8):
        sa.append(random.choice(seed))
    salt = ''.join(sa)
    print salt

    i = True
    if i is True:
        print 'true'
    else:
        print 'false'




