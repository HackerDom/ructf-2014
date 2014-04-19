FROM ructf2014:arch
MAINTAINER a799853@gmail.com

RUN useradd chat
ADD lib /usr/lib/
ADD chat /home/chat/

RUN chown -R chat:chat /home/chat

WORKDIR /home/chat
USER chat 

CMD /home/chat/chat

EXPOSE 5555
