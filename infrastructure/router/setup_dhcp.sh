#!/bin/bash -x

iface=${1?no iface}
N=${2?no number of VLANs}

ifaces=($(for i in $(seq 101 $((100+N))); do echo -n "$iface.$i "; done))
ifspec=${ifaces[@]}
sed -i "s/^INTERFACES=.*/INTERFACES=\"${ifspec// /\\ }\"/" /etc/default/isc-dhcp-server

python $(dirname $0)/gen_dhcp.py $N > /etc/dhcp/dhcpd.conf
/etc/init.d/isc-dhcp-server restart
