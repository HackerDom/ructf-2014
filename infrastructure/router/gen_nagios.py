#!/usr/bin/env python

# python gen_nagios.py 16 > /etc/nagios3/conf.d/ructf2014.cfg && /etc/init.d/nagios3 reload

import jinja2, sys

if __name__ == '__main__':
    N=sys.argv[1]
    template="""
define host {
    use            generic-host
    host_name      Core switch
    address        10.24.0.1
    check_interval 1
}

define command {
    command_name   check_snmp_port
    command_line   /usr/lib/nagios/plugins/check_snmp -H '$HOSTADDRESS$' -P 2c -C public -o 'IF-MIB::ifOperStatus.$ARG1$' -c 1
}

define servicegroup {
    servicegroup_name   Essential services
}

define hostgroup {
    hostgroup_name   Essential hosts
}

{% for i in range(1,N+1) %}
define host {
    use            generic-host
    host_name      Team {{i}} vuln image
    address        10.24.{{i}}.3
    check_interval 1
    hostgroups     Essential hosts
}

define host {
    use                   generic-host
    host_name             Team {{i}} test image
    address               10.24.{{i}}.2
    check_interval        1
    notifications_enabled 0
}

define host {
    use            generic-host
    host_name      Team {{i}} switch
    address        10.24.{{i}}.1
    check_interval 1
    hostgroups     Essential hosts
}

define service {
    use                 generic-service
    host_name           Team {{i}} switch
    service_description Core switch port
    check_command       check_snmp_port!124
    check_interval 1
    servicegroups       Essential services
}

define service {
    use                 generic-service
    host_name           Team {{i}} switch
    service_description Backup core switch port
    check_command       check_snmp_port!123
    check_interval 1
}

define service {
    use                 generic-service
    host_name           Team {{i}} switch
    service_description Reserved port #1
    check_command       check_snmp_port!101
    check_interval 1
}

define service {
    use                 generic-service
    host_name           Team {{i}} switch
    service_description Reserved port #2
    check_command       check_snmp_port!102
    check_interval 1
}

{% for j in range(3,23) %}
define service {
    use                   generic-service
    host_name             Team {{i}} switch
    service_description   Team port #{{j}}
    check_command         check_snmp_port!{{100+j}}
    check_interval 1
    notifications_enabled 0
}
{% endfor %}

{% endfor %}
    """

    print jinja2.Template(template).render(N=int(N))
