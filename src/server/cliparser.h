/*
 * cliparser.h
 * Server-mode CLI frontend patch for QSanguosha-Hegemony.
 *
 * Author: Li Yunfan
 * Licenced as part of the project under GNU GPL 3.0.
 */

#ifndef CLIPARSER_H
#define CLIPARSER_H

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "settings.h"

class CLIParser : public QObject
{
    Q_OBJECT

public:
    CLIParser(QCoreApplication *coreApp);

    void UpdateSettings(Settings &srvSettings);

private:
    QCommandLineOption
        serverName, serverAddr, serverPort, playerTimeout, numPlayers, banPackages,
        isFixSeat, numLuckCards, numMaxCards, isRewardFirstShow;
    QCommandLineParser cmdParser;
};

#endif // CLIPARSER_H
