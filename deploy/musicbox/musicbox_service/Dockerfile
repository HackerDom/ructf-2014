FROM ructf2014:arch
MAINTAINER dqkrait@gmail.com

RUN pacman -S --noconfirm libvorbis python2-pip
RUN pip2 install bottle

RUN useradd musicbox
ADD musicbox /home/musicbox/


RUN chown -R musicbox:musicbox /home/musicbox
USER musicbox

EXPOSE 4242 16780

WORKDIR /home/musicbox
CMD /home/musicbox/start.sh
