#!/bin/bash -e

BASE=$(dirname $0)

apt-get -y install $(cat $BASE/pkgs)

set +e;
/usr/bin/augtool -s set /files/etc/ssh/ssh_config/Host/ForwardAgent yes
/usr/bin/augtool -s set /files/etc/ssh/sshd_config/UseDNS no
/usr/bin/augtool -s set /files/etc/ssh/sshd_config/PasswordAuthentication no
/usr/bin/augtool -s set /files/etc/ssh/sshd_config/AllowAgentForwarding yes
set -e;
/etc/init.d/ssh restart

CPUSPEED=1600000

sed -e 's/^ENABLE=.*/ENABLE=true/' \
    -e 's/^GOVERNOR=.*/GOVERNOR=userspace/' \
    -e "s/^MIN_SPEED=.*/MIN_SPEED=$CPUSPEED/" \
    -e "s/^MAX_SPEED=.*/MAX_SPEED=$CPUSPEED/" \
    -i /etc/init.d/cpufrequtils
/etc/init.d/cpufrequtils restart
