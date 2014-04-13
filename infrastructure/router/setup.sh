#!/bin/bash -e

BASE=$(dirname $0)

PATH=/usr/bin:$PATH

$BASE/../common/setup.sh

apt-get -y install $(cat $BASE/pkgs)

set +e;
augtool -s set "/files/etc/aliases/*[name = 'root']/value" andrey.malets@gmail.com
set -e;

N=16

python $BASE/gen_dhcp.py $N > /etc/dhcp/dhcpd.conf
/etc/init.d/isc-dhcp-server restart

cp $BASE/check_testimage /usr/lib/nagios/plugins
python $BASE/gen_nagios.py $N > /etc/nagios3/conf.d/ructf2014.cfg
/etc/init.d/nagios3 reload

cp $BASE/iptables/init /etc/init.d/iptables
cp $BASE/iptables/default /etc/default/iptables
mkdir -p /var/lib/iptables

$BASE/gen_iptables.sh $N 'eth0.300 wlan0'
/etc/init.d/iptables save active
