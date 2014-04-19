FROM ructf2014:arch
MAINTAINER dqkrait@gmail.com

RUN pacman -Sy
RUN pacman -S --noconfirm icecast mpd mpc

ADD mpd.conf /etc/mpd.conf
ADD icecast.xml /etc/icecast.xml
ADD info.xsl /usr/share/icecast/web/info.xsl
ADD start.sh /bin/start.sh

EXPOSE 17216

CMD /bin/start.sh
