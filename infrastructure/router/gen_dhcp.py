#!/usr/bin/env python

import jinja2, sys

if __name__ == '__main__':
    N=int(sys.argv[1])
    vulnimage = 'c0:b0:11:22:33:44'
    testimage = 'c0:b0:11:22:33:00'

    template="""
option domain-name-servers 8.8.8.8;
authorative;
default-lease-time 600;
ddns-update-style none;
{% for i in range(1,N+1) %}
subnet 10.23.{{i}}.0 netmask 255.255.255.0 {
    range 10.23.{{i}}.10 10.23.{{i}}.254;
    option routers 10.23.{{i}}.1;
}
{% endfor %}
{% for i in range(1,N+1) %}
host testimage{{ loop.index }} {
    fixed-address 10.23.{{ loop.index }}.2;
    hardware ethernet {{ testimage }};
    option host-name test{{ loop.index }};
}
{% endfor %}
{% for i in range(1,N+1) %}
host vulnimage{{ loop.index }} {
    fixed-address 10.23.{{ loop.index }}.3;
    hardware ethernet {{ vulnimage }};
    option host-name team{{ loop.index }};
}
{% endfor %}
    """

    print jinja2.Template(template).render(vulnimage=vulnimage,
                                           testimage=testimage,
                                           N=N)
