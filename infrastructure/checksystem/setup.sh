#!/bin/bash -e

BASE=$(dirname $0)

PATH=/usr/bin:$PATH

$BASE/../common/setup.sh

apt-get -y install $(cat $BASE/pkgs)
