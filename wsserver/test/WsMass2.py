import sys
import time
import threading


class MyThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        time.sleep(1)
        print 'run finish'

def return_max_threads(num):
    threads = []

    for i in range(1, (num + 1)):
        threads.append(MyThread())

    for thread in threads:
        try:
            thread.start()
        except BaseException:
            print "Max number of threads is: \"%s\"" % thread.getName().split('-')[1]
            sys.exit(1)

    print "Max number of threads over: \"%s\"" % len(threads)

    for thread in threads:
        thread.join()


if __name__ == "__main__":
    if len(sys.argv[1:]) != 1:
        print "%s <thread_nums>" % sys.argv[0]
        sys.exit(1)
    arg = ''.join(sys.argv[1:])
    try:
        int(arg)
    except ValueError:
        print "please enter an \"integer\""
        sys.exit(1)
    num = int(arg)
    if num <= 1:
        print "please enter an integer greater than 1"
        sys.exit(1)
    return_max_threads(num)