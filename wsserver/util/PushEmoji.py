# coding=utf-8
'''
case
smile = 0 // 微笑
case
despise = 1 // 看不起，鄙视
case
thumb = 2 // 点赞
case
rolledEye = 3 // 白眼
case
sob = 4 // 啜泣
case
cry = 5 // 大哭
case
angry = 6 // 生气
case
speechless = 7 // 捂脸哭
'''
emoji_dict = {'0': '[微笑]', '1': '[鄙视]', '2': '[点赞]', '3': '[白眼]',
              '4': '[啜泣]', '5': '[大哭]', '6': '[生气]', '7': '[捂脸哭]'}


def getPushContent(content):
    try:
        if 'honey://emoji/' in content:
            content = content.replace('honey://emoji/', '')
            return emoji_dict[content]
        else:
            return content
    except Exception, e:
        print e
        return content

if __name__ == '__main__':
    print getPushContent('honey://emoji/5')