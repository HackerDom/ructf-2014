#!/usr/bin/env python

import os, re, sys, urllib2, urllib, zlib
from cookielib import CookieJar

def print_sample(host, pwd):
    def get_page(path, post=None):
        url = "http://%s/%s" % (host, path)
        url_opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(CookieJar()))
        data = (url_opener.open(url, urllib.urlencode(post)).read()
                if post != None
                else url_opener.open(url).read())
        try:
            return zlib.decompress(data, 16)
        except:
            return data

    def submit(path, args):
        args.update({'_submit' : 'Apply', 'btnSaveSettings' : 'APPLY'})
        return get_page(path, args)

    get_page('login.html', {'password' : pwd})

    print submit('system/system_name.html', {'nm' : 'Test name 4',
                                             'location' : 'Test location 4',
                                             'contact' : 'Test contact 4'})
    print get_page('status/status_ov.html')
    get_page('logout.html', {'tmp' : 0})

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print >> sys.stderr, "usage: %s <host> <pwd>" % sys.argv[0]
        sys.exit(1)
    host, pwd = sys.argv[1:3]
    print_sample(host, pwd)
