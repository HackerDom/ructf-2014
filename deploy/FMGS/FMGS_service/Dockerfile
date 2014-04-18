FROM ructf2014:arch
MAINTAINER victor.samun@gmail.com

RUN useradd fmgs

ADD FMGS /home/FMGS
RUN chown -R fmgs:fmgs /home/FMGS
USER fmgs

WORKDIR /home/FMGS
ENTRYPOINT /home/FMGS/FMGS.pl

EXPOSE 44100
