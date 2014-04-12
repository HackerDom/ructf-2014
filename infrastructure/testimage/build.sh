#!/bin/bash -e

dir=${1?no build dir}

mkdir -p $dir
pushd $dir

rm -rf * .build
lb config -b iso -a i386 \
  --apt-indices false --debian-installer false --apt-source-archives false \
  --debootstrap-options --variant=minbase -m http://mirror.yandex.ru/debian/

put_in_config() {
  path=${1? no path}
  file=config/$path
  mkdir -p $(dirname $file)
  cat > $file
}

echo vim-tiny ifupdown isc-dhcp-client nginx openbsd-inetd atftpd | \
  put_in_config package-lists/ructf.list.chroot_live

put_in_config includes.chroot/etc/network/interfaces <<END
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet dhcp
END

put_in_config includes.chroot/etc/nginx/sites-enabled/default <<END
server {
  listen 80 default;
  location / {
    root /srv/tftp;
  }
}
END

ln -sf /usr/share/doc/live-build/examples/hooks/stripped.chroot config/hooks
put_in_config hooks/ructf.hook.chroot <<END
sed -i 's/^[123456]:.*$//g' /etc/inittab
echo 'send dhcp-client-identifier 1:c0:b0:11:22:33:00;' >> /etc/dhcp/dhclient.conf
END

put_in_config includes.binary/isolinux/isolinux.cfg <<END
include menu.cfg
default vesamenu.c32
prompt 0
timeout 1
END

lb build

popd
