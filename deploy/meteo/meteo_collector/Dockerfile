FROM ructf2014:arch
MAINTAINER andgein@yandex.ru

EXPOSE 9000

RUN pacman -S --noconfirm python3 python-pip
RUN pip3 install pymysql

ADD collector.py /home/collector/

RUN useradd collector
RUN chown -R collector:collector /home/collector

WORKDIR /home/collector
USER collector

CMD python3 collector.py
 