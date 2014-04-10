#!/bin/bash -e

BASE=$(dirname $0)

PATH=/usr/bin:$PATH

$BASE/../common/setup.sh

apt-get -y install $(cat $BASE/pkgs)

N=16

python $BASE/gen_dhcp.py $N > /etc/dhcp/dhcpd.conf
/etc/init.d/isc-dhcp-server restart

python gen_nagios.py $N > /etc/nagios3/conf.d/ructf2014.cfg
/etc/init.d/nagios3 reload
