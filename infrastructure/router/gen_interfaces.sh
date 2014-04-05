#!/bin/bash

iface=${1?no iface}
N=${2?no number of VLANs}

cat <<END
auto lo
iface lo inet loopback

# Wireless internet on the game (default)
auto wlan0
iface wlan0 inet dhcp
	pre-up rfkill unblock all
	wpa-conf /etc/wpa_supplicant.conf

auto $iface
iface $iface inet manual

# Wired internet on the game (disabled by default)
iface $iface.300 inet dhcp

END

for i in $(seq 1 $N); do cat <<END

auto $iface.$((100+i))
iface $iface.$((100+i)) inet static
    address 10.23.$i.1
    netmask 255.255.255.0

auto $iface.$((200+i))
iface $iface.$((200+i)) inet static
    address 10.24.$i.254
    netmask 255.255.255.0

END
done
