FROM ructf2014:arch
MAINTAINER andgein@yandex.ru

RUN pacman -S --noconfirm mysql

ADD init_sql /tmp/init_sql
RUN /tmp/init_sql/init_sql.sh

EXPOSE 3306

CMD ["/usr/bin/mysqld_safe"]
