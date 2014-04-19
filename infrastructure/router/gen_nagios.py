#!/usr/bin/env python

# python gen_nagios.py 16 > /etc/nagios3/conf.d/ructf2014.cfg && /etc/init.d/nagios3 reload

import jinja2, sys

if __name__ == '__main__':
    N=sys.argv[1]
    template="""
define command {
    command_name   check_snmp_port
    command_line   /usr/lib/nagios/plugins/check_snmp -H '$HOSTADDRESS$' -P 2c -C public -o 'IF-MIB::ifOperStatus.$ARG1$' -c 1
}

define command {
    command_name   check_snmp_portspeed
    command_line   /usr/lib/nagios/plugins/check_snmp -H '$HOSTADDRESS$' -P 2c -C public -o 'IF-MIB::ifSpeed.$ARG1$' -c 1000000000
}

define command {
    command_name   check_testimage
    command_line   /usr/lib/nagios/plugins/check_testimage $HOSTADDRESS
}

define servicegroup {
    servicegroup_name   Essential services
}

define hostgroup {
    hostgroup_name   Essential hosts
}

define host {
    use            generic-host
    host_name      Checksystem
    address        10.23.0.2
    hostgroups     Essential hosts
    check_interval 2
}

define host {
    use            generic-host
    host_name      hewlett
    address        10.23.0.3
    hostgroups     Essential hosts
    check_interval 2
}

define host {
    use            generic-host
    host_name      packard
    address        10.23.0.4
    hostgroups     Essential hosts
    check_interval 2
}

define host {
    use            generic-host
    host_name      Core switch
    address        10.24.0.1
    hostgroups     Essential hosts
    check_interval 2
}

{% for i in range(1,N+1) %}
{% if i != 16 %}
define host {
    use            generic-host
    host_name      Team {{i}} vuln image
    address        10.23.{{i}}.3
    check_interval 2
    hostgroups     Essential hosts
}

define service {
    use                 generic-service
    host_name           Team {{i}} vuln image
    service_description SSH
    check_command       check_ssh
    check_interval 2
}

define host {
    use                   generic-host
    host_name             Team {{i}} test image
    address               10.23.{{i}}.2
    check_interval        2
    notifications_enabled 0
}

define service {
    use                 generic-service
    host_name           Team {{i}} test image
    service_description tftp to http
    check_command       check_testimage
    check_interval 2
}
{% endif %}

define host {
    use            generic-host
    host_name      Team {{i}} switch
    address        10.24.{{i}}.1
    check_interval 2
    hostgroups     Essential hosts
}

define service {
    use                 generic-service
    host_name           Core switch
    service_description Team {{i}} port
    check_command       check_snmp_port!{{i}}
    check_interval 2
    servicegroups       Essential services
}

define service {
    use                 generic-service
    host_name           Core switch
    service_description Team {{i}} port speed
    check_command       check_snmp_portspeed!{{i}}
    check_interval 2
    servicegroups       Essential services
}

define service {
    use                 generic-service
    host_name           Team {{i}} switch
    service_description Core switch port
    check_command       check_snmp_port!124
    check_interval 2
    servicegroups       Essential services
}

define service {
    use                 generic-service
    host_name           Team {{i}} switch
    service_description Core switch port speed
    check_command       check_snmp_portspeed!124
    check_interval 2
    servicegroups       Essential services
}

define service {
    use                 generic-service
    host_name           Team {{i}} switch
    service_description Backup core switch port
    check_command       check_snmp_port!102
    check_interval 2
}

define service {
    use                 generic-service
    host_name           Team {{i}} switch
    service_description Backup core switch port speed
    check_command       check_snmp_portspeed!102
    check_interval 2
}

{% for j in range(1,23) %}
define service {
    use                   generic-service
    host_name             Team {{i}} switch
    service_description   Team port #{{j}}
    check_command         check_snmp_port!{{100+j}}
    check_interval 2
    notifications_enabled 0
}
{% endfor %}

{% endfor %}
    """

    print jinja2.Template(template).render(N=int(N))
