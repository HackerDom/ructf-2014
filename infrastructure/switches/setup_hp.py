#!/usr/bin/env python

import os, re, socket, sys, urllib2, urllib, zlib
from cookielib import CookieJar

def get_page(host, path, post=None):
    url = "http://%s/%s" % (host, path)
    url_opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(CookieJar()))
    timeout = 3
    data = (url_opener.open(url, urllib.urlencode(post), timeout).read()
            if post != None
            else url_opener.open(url).read())
    try:
        return zlib.decompress(data, 16)
    except:
        return data

def submit(host, path, args):
    args.update({'_submit' : 'Apply', 'btnSaveSettings' : 'APPLY'})
    return get_page(host, path, args)

def port2field(base, port):
    return 'R%x' % (int(port) + base)

def portspec2list((base, portspec)):
    return dict(map(lambda port: [port2field(base, port)] * 2,
                    portspec.split(',')))

def portspec2enabled((base, count, with0, portspec)):
    result = {}
    if with0:
        for port in range(1, count+1):
            result[port2field(base, port)] = 0
    for port in portspec.split(','):
        if len(port) != 0:
            result[port2field(base, port)] = 1
    return result

cmds = {}
def cmd(login=False):
    def rv(fn):
        def result(host, pwd, args):
            if login:
                get_page(host, 'login.html', {'password' : pwd})
                fn(host, *args)
                get_page(host, 'logout.html', {'tmp' : 0})
            else:
                fn(host, pwd, *args)
        cmds[fn.__name__] = result
        return fn
    return rv

@cmd()
def reset(host, pwd):
    get_page(host, 'login.html', {'password' : pwd})
    try:
        submit(host, 'system/system_config.html',
               {'factory' : 'Yes',
                'warm' : '',
                'tool_sel' : 'reset',
                'upload' : 'on',
                'R10' : 0})
    except socket.timeout:
        pass

@cmd()
def change_pwd(host, old, new):
    get_page(host, 'login.html', {'password' : old})
    submit(host, 'system/system_pswd.html', {'pow' : old, 'pw' : new, 'pcw' : new})
    get_page(host, 'logout.html', {'tmp' : 0})

@cmd()
def change_ip(host, pwd, ip, mask, gw):
    get_page(host, 'login.html', {'password' : pwd})
    ips, masks, gws = map(lambda name: name.split('.'), [ip, mask, gw])
    params = {'ip' : ip, 'sm' : mask, 'gw' : gw}
    for i in range(1, 5):
        params['lanip_%d' % i] = ips[i-1]
        params['sn_%d' % i]    = masks[i-1]
        params['gw_%d' % i]    = gws[i-1]
    submit(host, 'system/system_ls.html', params)

@cmd(login=True)
def change_vlan(host, vlan, portspec):
    args = {'submit' : 'Apply'}
    args.update(portspec2list((15, portspec)))
    get_page(host, 'vlans/vlan_setup.html', args)

@cmd(login=True)
def add_vlan(host, vlan, portspec):
    get_page(host, 'vlans/vlan_mconf.html',
                   {'VID' : vlan,
                    'submit' : 'Add',
                    'R10' : 0}) # does not matter
    change_vlan(host, vlan, portspec)

@cmd(login=True)
def del_vlan(host, vlan):
    data = get_page(host, 'vlans/vlan_mconf.html')
    vlans = {}
    for i in range(0, 64):
        res = re.search('<input\s+name="R10"\s+'
                        'type="radio"\s+'
                        'value="%d"\s*(?:checked\s*)?><br>'
                        '\s*(\d+)</td>' % i, data)
        if res != None:
            vlans[res.group(1)] = i
    num = vlans[vlan]
    get_page(host, 'vlans/vlan_mconf.html',
             {'submit' : 'Delete',
              'VID' : '',
              'R10' : num})

@cmd(login=True)
def configure_ports(host, aware, ingress, tagged, pvid):
    PORT_COUNT = 24
    if len(pvid.split(',')) != PORT_COUNT:
        raise Exception('pvid list should have length of exactly 24')
    args = {'_submit' : 'Apply'}
    for part in map(portspec2enabled, [
            (0xf,  PORT_COUNT, False, aware),
            (0x4f, PORT_COUNT, False, ingress),
            (0x8f, PORT_COUNT, True,  tagged)]):
        args.update(part)
    pvids = pvid.split(',')
    for port in range(0, len(pvids)):
        args['pvid%d' % (port+1)] = pvids[port]
    get_page(host, 'vlans/vlan_pconf.html', args)

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
