FROM ructf2014:arch
MAINTAINER bay@hackerdom.ru

RUN pacman -S --noconfirm python2 xinetd
RUN useradd simple

ADD simple /etc/xinetd.d/simple
ADD simple.py /home/simple/simple.py

EXPOSE 10000

ENTRYPOINT /usr/sbin/xinetd -dontfork
