# QSanguosha-Hegemony-Server

Game server of QSanguosha-for-Hegemony, a popular card game in China. This server is based on Mogara's QSanguosha-For-Hegemony repository, game version 2. It is compatible with game clients built from there.

# Usage
Example of running the game server with default configuration in background:
```
$ docker --publish 9527:9527 --detach --name sgs-srv lyftfc/qsanguosha-hegemony-server
```

View the help of server program: (note that the container only exposes port 9527 by default)
```
$ docker -it lyftfc/qsanguosha-hegemony-server QSanguosha-Server --help
```
Arguments specified in the command-line will override those in the default configuration.

Run after editing configuration:
```
$ docker --publish 9527:9527 -i -t --name sgs-srv lyftfc/qsanguosha-hegemony-server QSanguosha-Server-Config
```
The configuration file is under `/opt/QSanguosha-Hegemony-Server/default.conf`.

# Source Repository

Mogara's original repository: https://github.com/Mogara/QSanguosha-For-Hegemony

Adapted for server with CLI interface: https://github.com/thefoggycity/QSanguosha-For-Hegemony

# License
Refer to the source repository for license description.
