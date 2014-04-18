FROM ructf2014:arch
MAINTAINER a799853@gmail.com

RUN pacman -S --noconfirm mongodb
RUN mkdir -p /data/db

EXPOSE 27017 28017

CMD ["/usr/sbin/mongod"]
