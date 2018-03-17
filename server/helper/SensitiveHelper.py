#coding=utf-8
from mecloud.lib import log
from mecloud.helper.ClassHelper import *
from mecloud.model.MeQuery import *
import time
import jieba


class cNode(object):
    def __init__(self):
        self.children = None


# The encode of word is UTF-8
# The encode of message is UTF-8
class cDfa(object):
    def __init__(self, lWords):
        self.root = None
        self.root = cNode()
        for sWord in lWords:
            self.addWord(sWord)

    # The encode of word is UTF-8
    def addWord(self, word):
        node = self.root
        iEnd = len(word) - 1
        for i in xrange(len(word)):
            if node.children == None:
                node.children = {}
                if i != iEnd:
                    node.children[word[i]] = (cNode(), False)
                else:
                    node.children[word[i]] = (cNode(), True)

            elif word[i] not in node.children:
                if i != iEnd:
                    node.children[word[i]] = (cNode(), False)
                else:
                    node.children[word[i]] = (cNode(), True)
            else:  # word[i] in node.children:
                if i == iEnd:
                    Next, bWord = node.children[word[i]]
                    node.children[word[i]] = (Next, True)

            node = node.children[word[i]][0]

    def isContain(self, sMsg):
        root = self.root
        iLen = len(sMsg)
        for i in xrange(iLen):
            p = root
            j = i
            while (j < iLen and p.children != None and sMsg[j] in p.children):
                (p, bWord) = p.children[sMsg[j]]
                if bWord:
                    return True
                j = j + 1
        return False

    def filter(self, sMsg):
        lNew = []
        root = self.root
        iLen = len(sMsg)
        i = 0
        bContinue = False
        while i < iLen:
            p = root
            j = i
            while (j < iLen and p.children != None and sMsg[j] in p.children):
                (p, bWord) = p.children[sMsg[j]]
                if bWord:
                    # print sMsg[i:j+1]
                    lNew.append(u'*' * (j - i + 1))  # keyword替换
                    return sMsg[i:j+1]
                    # i = j + 1
                    # bContinue = True
                    # break
                j = j + 1
            if bContinue:
                bContinue = False
                continue
            lNew.append(sMsg[i])
            i = i + 1
        # return u''.join(lNew)
        return False



sensitiveWord = {}
df = None

class SensitiveHelper:
    replaceWord = "666666"
    def __init__(self, sentence,level=None):
        global sensitiveWord
        global df
        self.sentence = sentence
        self.sentenceCut = list(jieba.cut(self.sentence))
        if not sensitiveWord:
            sensitiveWordHelper = ClassHelper("SensitiveWord")
            items = sensitiveWordHelper.find({})
            for item in items:
                sensitiveWord[item['word']] = item
            df = cDfa(sensitiveWord.keys())

    def filterWord(self,userId):
        # print self.sentenceCut
        # localSentence = self.sentence
        # print self.sentenceCut
        # for word in self.sensitiveWord:
        #     # for cutWord in self.sentenceCut:
        #     #     if len(cutWord) < 2:#两个字
        #     #         continue
        #     item = self.sensitiveWord[word]
        #     if word in self.sentenceCut:
        #         # localSentence = self.sentence.replace(word['word'],"666666")
        #         self.sentence = self.replaceWord
        #         if item['level'] in [1,2,3]:
        #             classHelper = ClassHelper("Violations")
        #             classHelper.create({
        #                 "party":userId,
        #                 "informer":"system",
        #                 "from":1,
        #                 "type":item['type'],
        #                 "detail":item["word"],
        #                 "acl":{"*":{'read':True}}
        #             })
        #         return self.sentence
        # return self.sentence

        word = df.filter(self.sentence)
        if word:
            self.sentence = self.replaceWord
            item = sensitiveWord[word]
            if item['level'] in [1, 2, 3]:
                classHelper = ClassHelper("Violations")
                classHelper.create({
                    "party":userId,
                    "informer":"system",
                    "from":1,
                    "type":item['type'],
                    "detail":item["word"],
                    "acl":{"*":{'read':True}}
                })
        return self.sentence




