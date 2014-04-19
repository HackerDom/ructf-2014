FROM ructf2014:arch
MAINTAINER andgein@yandex.ru

EXPOSE 8888

RUN pacman -S --noconfirm nodejs
RUN npm install mysql ejs

ADD meteo /home/meteo
RUN useradd meteo
RUN chown -R meteo:meteo /home/meteo

USER meteo

WORKDIR /home/meteo
CMD node server.js

