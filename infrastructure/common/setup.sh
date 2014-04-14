#!/bin/bash -e

BASE=$(dirname $0)

PATH=/usr/bin:$PATH

apt-get -y install $(cat $BASE/pkgs)

set +e;
augtool -s set /files/etc/ssh/ssh_config/Host/ForwardAgent yes
augtool -s set /files/etc/ssh/sshd_config/UseDNS no
augtool -s set /files/etc/ssh/sshd_config/PasswordAuthentication no
augtool -s set /files/etc/ssh/sshd_config/AllowAgentForwarding yes
set -e;
/etc/init.d/ssh restart

CPUSPEED=1600000

sed -e 's/^ENABLE=.*/ENABLE=true/' \
    -e 's/^GOVERNOR=.*/GOVERNOR=userspace/' \
    -e "s/^MIN_SPEED=.*/MIN_SPEED=$CPUSPEED/" \
    -e "s/^MAX_SPEED=.*/MAX_SPEED=$CPUSPEED/" \
    -i /etc/init.d/cpufrequtils
/etc/init.d/cpufrequtils restart

cat > /etc/cron-apt/action.d/3-download <<END
autoclean -y
upgrade -y -o APT::Get::Show-Upgraded=true
END
