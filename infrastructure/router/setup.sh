#!/bin/bash -e

BASE=$(dirname $0)

PATH=/usr/bin:$PATH

$BASE/../common/setup.sh

apt-get -y install $(cat $BASE/pkgs)

set +e;
augtool -s set /files/etc/ssh/sshd_config/ListenAddress[1] 10.23.0.1
augtool -s set /files/etc/ssh/sshd_config/ListenAddress[2] 172.16.19.178
set -e;
/etc/init.d/ssh restart

set +e;
augtool -s set "/files/etc/aliases/*[name = 'root']/value" andrey.malets@gmail.com
set -e;

N=16

python $BASE/gen_dhcp.py $N > /etc/dhcp/dhcpd.conf
/etc/init.d/isc-dhcp-server restart

cp $BASE/check_testimage /usr/lib/nagios/plugins
python $BASE/gen_nagios.py $N > /etc/nagios3/conf.d/ructf2014.cfg
/etc/init.d/nagios3 reload
