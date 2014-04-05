#!/bin/bash

N=${1?no team count}
[ -n "$2" ] && clean=true || clean=false
cat <<END
config ipif system ipaddress 10.24.0.1/24 vlan default 
END

$clean || echo "delete vlan JURY"
echo "create vlan JURY tag 100"
echo "config vlan JURY add untagged 1:(23)"
echo "config vlan JURY add tagged 1:(24)"

$clean || echo "delete vlan EXTERNAL"
echo "create vlan EXTERNAL tag 300"
echo "config vlan EXTERNAL add untagged 1:(21-22)"
echo "config vlan EXTERNAL add tagged 1:(24)"

for i in $(seq 1 $N); do
    $clean || echo "delete vlan TEAM$i"
    echo "create vlan TEAM$i tag $((100+i))"
    echo "config vlan TEAM$i add tagged 1:($i)"
    echo "config vlan TEAM$i add tagged 1:(24)"
done

for i in $(seq 1 $N); do
    $clean || echo "delete vlan RTEAM$i"
    echo "create vlan RTEAM$i tag $((200+i))"
    echo "config vlan RTEAM$i add untagged 1:($i)"
    echo "config vlan RTEAM$i add tagged 1:(24)"
done
