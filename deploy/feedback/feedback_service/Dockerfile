FROM ructf2014:arch
MAINTAINER mail.dmitriy@gmail.com

RUN pacman -S --noconfirm wget
RUN wget 'https://alexbers.com/java/jdk-8u20-ea-b10-linux-arm-vfp-hflt-15_apr_2014.tar.gz' -O - | tar xzf - -C /opt/

RUN useradd feedback

ADD feedback /home/feedback
RUN chown -R feedback:feedback /home/feedback
USER feedback
WORKDIR /home/feedback

CMD /opt/jdk1.8.0_20/bin/java -XX:+AggressiveOpts -Xms128m -Xmx320m -server -jar feedback.jar 7654

EXPOSE 7654
