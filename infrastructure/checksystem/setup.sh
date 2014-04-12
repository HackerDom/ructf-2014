#!/bin/bash -e

BASE=$(dirname $0)

PATH=/usr/bin:$PATH

$BASE/../common/setup.sh

apt-get -y install $(cat $BASE/pkgs)

set +e;
augtool -s set "/files/etc/aliases/*[name = 'root']/value" avkhozov@gmail.com
set -e;
