#!/usr/bin/env python

import os, re, sys, urllib2, urllib, zlib
from cookielib import CookieJar

def get_page(host, path, post=None):
    url = "http://%s/%s" % (host, path)
    url_opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(CookieJar()))
    data = (url_opener.open(url, urllib.urlencode(post)).read()
            if post != None
            else url_opener.open(url).read())
    try:
        return zlib.decompress(data, 16)
    except:
        return data

def submit(host, path, args):
    args.update({'_submit' : 'Apply', 'btnSaveSettings' : 'APPLY'})
    return get_page(host, path, args)

cmds = {}
def cmd(fn):
    cmds[fn.__name__] = lambda host, pwd, args: fn(host, pwd, *args)

@cmd
def change_pwd(host, old, new):
    get_page(host, 'login.html', {'password' : old})
    submit(host, 'system/system_pswd.html', {'pow' : old, 'pw' : new, 'pcw' : new})
    get_page(host, 'logout.html', {'tmp' : 0})

@cmd
def change_ip(host, pwd, ip, mask, gw):
    get_page(host, 'login.html', {'password' : pwd})
    ips, masks, gws = map(lambda name: name.split('.'), [ip, mask, gw])
    params = {'ip' : ip, 'sm' : mask, 'gw' : gw}
    for i in range(1, 5):
        params['lanip_%d' % i] = ips[i-1]
        params['sn_%d' % i]    = masks[i-1]
        params['gw_%d' % i]    = gws[i-1]
    submit(host, 'system/system_ls.html', params)
    #get_page(host, 'logout.html', {'tmp' : 0})

@cmd
def add_vlan(host, pwd, vlan, portspec):
    get_page(host, 'login.html', {'password' : pwd})
    get_page(host, 'vlans/vlan_mconf.html',
                   {'VID' : vlan,
                    'submit' : 'Add',
                    'R10' : 0}) # does not matter
    args = {'submit' : 'Apply'}
    args.update(dict(map(lambda port: [('R%x' % (int(port) + 15))] * 2,
                         portspec.split(','))))
    get_page(host, 'vlans/vlan_setup.html', args)
    get_page(host, 'logout.html', {'tmp' : 0})

@cmd
def del_vlan(host, pwd, vlan):
    get_page(host, 'login.html', {'password' : pwd})
    data = get_page(host, 'vlans/vlan_mconf.html')
    vlans = {}
    for i in range(0, 64):
        res = re.search('<input\s+name="R10"\s+'
                        'type="radio"\s+'
                        'value="%d"\s*(?:checked\s*)?><br>'
                        '\s*(\d+)</td>' % i, data)
        if res != None:
            vlans[res.group(1)] = i
    try:
        num = vlans[vlan]
        get_page(host, 'vlans/vlan_mconf.html',
                 {'submit' : 'Delete',
                  'VID' : '',
                  'R10' : num})
    finally:
        get_page(host, 'logout.html', {'tmp' : 0})

@cmd
def setup(host, pwd):
    get_page(host, 'login.html', {'password' : pwd})
    # TODO(malets): do some stuff here
    get_page(host, 'logout.html', {'tmp' : 0})

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print >> sys.stderr, cmds.keys()
        sys.exit(1)

    cmd, host, pwd, args = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4:]

    if cmd in cmds:
        cmds[cmd](host, pwd, args)
    else:
        print >> sys.stderr, cmds.keys()
        sys.exit(1)
