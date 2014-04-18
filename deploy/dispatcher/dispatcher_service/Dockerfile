FROM ructf2014:arch
#FROM localrepo:arch
MAINTAINER bay@hackerdom.ru

RUN pacman -Sy
RUN pacman -S --noconfirm python2-pip
RUN pacman -S --noconfirm gcc
RUN pip2 install flask

RUN useradd dispatcher

ADD dispatcher /home/dispatcher
RUN chown -R dispatcher:dispatcher /home/dispatcher
USER dispatcher

ENTRYPOINT python2 /home/dispatcher/main.py

EXPOSE 5000
