# -*- coding: utf-8 -*-
import sys
import threading
import time
import traceback


class Counter:
    def __init__(self):
        self.lock = threading.Lock()
        self.value = 0

    def increment(self):
        self.lock.acquire()
        self.value = value = self.value + 1
        self.lock.release()
        return value


counter = Counter()
cond = threading.Condition()


class Worker(threading.Thread):
    def run(self):
        print self.getName(), "-- created."
        cond.acquire()
        cond.wait()
        cond.release()

#测试你的系统最多可以跑的线程数

if __name__ == '__main__':

    try:
        for i in range(11000):
            Worker().start()  # start a worker
    except Exception, e:
        print e
        msg = traceback.format_exc()
        print msg
        time.sleep(5)
        print "maxium i=", i
    finally:
        cond.acquire()
        cond.notifyAll()
        cond.release()
        time.sleep(5)
        print threading.currentThread().getName(), " quit"
        sys.exit()
