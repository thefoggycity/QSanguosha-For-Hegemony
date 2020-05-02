FROM lyftfc/fedora-qt5-gui:latest

WORKDIR /opt/
COPY . QSanguosha-Hegemony-Server
RUN mv ./QSanguosha-Hegemony-Server/start.sh /usr/local/bin/QSanguosha-Server
RUN mv ./QSanguosha-Hegemony-Server/conf.sh /usr/local/bin/QSanguosha-Server-Config
RUN chmod +x /usr/local/bin/QSanguosha-Server*

EXPOSE 9527

CMD [ "QSanguosha-Server" ]

