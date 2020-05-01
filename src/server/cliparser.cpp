/*
 * cliparser.cpp
 * Server-mode CLI frontend patch for QSanguosha-Hegemony.
 *
 * Author: Li Yunfan
 * Licenced as part of the project under GNU GPL 3.0.
 */

#include "cliparser.h"

CLIParser::CLIParser(QCoreApplication *coreApp) :
    serverName({"s", "server-name"},
               "Server name shown to the players.",
               "ServerName"),
    serverAddr({"a", "address"},
               "Server IP or domain address for connections.",
               "Address"),
    serverPort({"p", "port"},
               "Server port for connection listening.",
               "ServerPort"),
    playerTimeout({"t", "timeout"},
                  "Time limit for players' operation, in seconds.",
                  "OperationTimeout"),
    numPlayers({"n", "players"},
               "Number of players in the game, between 2 to 10.",
               "NumPlayers"),
    banPackages({"b", "ban-package"},
                "Card package not to be used in the game. Use the option multiple times for more packages.",
                "BanPackage"),
    isFixSeat({"f", "fixed-seat"},
              "Fix players seat position, otherwise randomized."),
    numLuckCards({"l", "luck-cards"},
                 "Number of luck card a player is allowed to use.",
                 "LuckCards"),
    numMaxCards({"c", "max-cards"},
                "Maximum number of cards that a player may hold.",
                "MaxCards"),
    isRewardFirstShow({"r", "reward-first-show"},
                      "Reward 2 cards to the player that reveals the character first.")
{
    cmdParser.setApplicationDescription("QSanguosha for Hegemony game server.");
    cmdParser.addVersionOption();
    cmdParser.addHelpOption();
    cmdParser.addOptions({
        serverName, serverAddr, serverPort, playerTimeout, numPlayers, banPackages,
        isFixSeat, numLuckCards, numMaxCards, isRewardFirstShow
    });
    cmdParser.process(*coreApp);
}

void CLIParser::UpdateSettings(Settings &srvSettings)
{
    if (cmdParser.isSet(serverName))
        srvSettings.ServerName = cmdParser.value(serverName);
    if (cmdParser.isSet(serverAddr))
        srvSettings.Address = cmdParser.value(serverAddr);
    if (cmdParser.isSet(serverPort))
        srvSettings.ServerPort = cmdParser.value(serverPort).toUInt();
    if (cmdParser.isSet(playerTimeout)) {
        int val = cmdParser.value(playerTimeout).toInt();
        if (val <= 0) {
            puts("Error: Timeout must be at least 1 second.");
            cmdParser.showHelp(1);
        } else
            srvSettings.OperationTimeout = val;
    }
    if (cmdParser.isSet(numPlayers)) {
        int val = cmdParser.value(numPlayers).toInt();
        if (val > 10 || val < 2) {
            puts("Error: player must be at least 2 but no more than 10.");
            cmdParser.showHelp(1);
        } else {
            QString mode = QString::number(val);
            if (val < 10)
                mode.prepend('0');
            mode.append('p');
            srvSettings.GameMode = mode;
        }
    }
    printf("Game mode is %s, timeout %d seconds.\n",
           srvSettings.GameMode.toStdString().c_str(),
           srvSettings.OperationTimeout);
    if (cmdParser.isSet(banPackages))
        srvSettings.BanPackages = cmdParser.values(banPackages);
    printf("Banned packages are:");
    for (auto bp : srvSettings.BanPackages)
        printf(" %s", bp.toStdString().c_str());
    puts("");
    if (cmdParser.isSet(isFixSeat))
        srvSettings.RandomSeat = false;
    if (cmdParser.isSet(numLuckCards))
        srvSettings.LuckCardLimitation = cmdParser.value(numLuckCards).toInt();
    if (cmdParser.isSet(numMaxCards))
        srvSettings.MaxCards = cmdParser.value(numMaxCards).toInt();
    if (cmdParser.isSet(isRewardFirstShow))
        srvSettings.RewardTheFirstShowingPlayer = true;
}
