#!/usr/bin/env python

import jinja2, sys

if __name__ == '__main__':
    N=sys.argv[1]
    #TODO(uzer): write cubies macs here
    cubies = [
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
        '00:00:00:00:00:00',
    ]

    template="""
option domain-name-servers 8.8.8.8;
authorative;
default-lease-time 600;
ddns-update-style none;
{% for i in range(1,N+1) %}
subnet 10.23.{{i}}.0 netmask 255.255.255.0 {
    range 10.23.{{i}}.10 10.23.{{i}}.253;
    option routers 10.23.{{i}}.1;
}
{% endfor %}
{% for i in range(1,N+1) %}
host test{{ loop.index }} {
    fixed-address 10.23.{{ loop.index }}.2;
    hardware ethernet 00:11:22:33:44:55;
    option host-name test{{ loop.index }};
}
{% endfor %}
{% for mac in cubies %}
host cubie{{ loop.index }} {
    fixed-address 10.23.{{ loop.index }}.3;
    hardware ethernet {{ mac }};
    option host-name team{{ loop.index }};
}
{% endfor %}
    """

    print jinja2.Template(template).render(cubies=cubies, N=int(N))
