/********************************************************************
    Copyright (c) 2013-2015 - Mogara

    This file is part of QSanguosha-Hegemony.

    This game is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3.0
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    See the LICENSE file for more details.

    Mogara
    *********************************************************************/

#if defined(WIN32) && !defined(GPP) && !defined(QT_NO_DEBUG) && !defined(WINRT)
#include <vld/vld.h>
#endif

#include <QFile>
#include <QCoreApplication>
#include <QTranslator>
#include <QDateTime>

#include "server.h"
#include "settings.h"
#include "engine.h"

#ifndef SRV_ONLY
#include <QApplication>
#include <QSplashScreen>
#include <QMessageBox>

#include "mainwindow.h"
#include "audio.h"
#include "stylehelper.h"
#else
#include <QCommandLineParser>
#include <QCommandLineOption>
#endif

#ifndef WINDOWS
#include <QDir>
#endif

#ifdef USE_BREAKPAD
#include <client/windows/handler/exception_handler.h>
#include <QProcess>

using namespace google_breakpad;

static bool callback(const wchar_t *, const wchar_t *id, void *, EXCEPTION_POINTERS *, MDRawAssertionInfo *, bool succeeded)
{
    if (succeeded && QFile::exists("QSanSMTPClient.exe")) {
        char ID[16000];
        memset(ID, 0, sizeof(ID));
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
        wcstombs(ID, id, wcslen(id));
#ifdef _MSC_VER
#pragma warning(pop)
#endif
        QProcess *process = new QProcess(qApp);
        QStringList args;
        args << QString(ID) + ".dmp";
        process->start("QSanSMTPClient", args);
    }
    return succeeded;
}
#endif

int main(int argc, char *argv[])
{
#ifdef SRV_ONLY
#ifndef Q_OS_UNIX
#warning "Server-only mode is tested for Unix platform only."
#endif  // Q_OS_UNIX
#define showSplashMessage(message) do{} while (false)
#define showSplashOrLog(str) puts(str)
    new QCoreApplication(argc, argv);
    QCoreApplication::setApplicationName("QSanguosha for Hegemony Server");
    QCoreApplication::setApplicationVersion(QString("2.0 Qt ").append(QT_VERSION_STR));
    QCommandLineParser cmdParser;
    cmdParser.setApplicationDescription("QSanguosha for Hegemony game server.");
    cmdParser.addVersionOption();
    cmdParser.addHelpOption();
    QCommandLineOption serverName({"s", "server-name"},
                                  "Server name shown to the players.",
                                  "ServerName");
    QCommandLineOption serverAddr({"a", "address"},
                                  "Server IP or domain address for connections.",
                                  "Address");
    QCommandLineOption serverPort({"p", "port"},
                                  "Server port for connection listening.",
                                  "ServerPort");
    QCommandLineOption playerTimeout({"t", "timeout"},
                                     "Time limit for players' operation, in seconds.",
                                     "OperationTimeout");
    QCommandLineOption numPlayers({"n", "players"},
                                  "Number of players in the game, between 2 to 10.",
                                  "NumPlayers");
    QCommandLineOption banPackages({"b", "ban-packages"},
                                   "Card packages not to be used in the game. Use multiple times for more packages.",
                                   "BanPackages");
    QCommandLineOption isFixSeat({"f", "fixed-seat"},
                                  "Fix players seat position, otherwise randomized.");
    QCommandLineOption numLuckCards({"l", "luck-cards"},
                                    "Number of luck card a player is allowed to use.",
                                    "LuckCards");
    QCommandLineOption numMaxCards({"c", "max-cards"},
                                   "Maximum number of cards that a player may hold.",
                                   "MaxCards");
    QCommandLineOption isRewardFirstShow({"r", "reward-first-show"},
                                         "Reward 2 cards to the player that reveals the character first.");
    QList<QCommandLineOption> cmdParserOpts = {
        serverName, serverAddr, serverPort, playerTimeout, numPlayers, banPackages,
        isFixSeat, numLuckCards, numMaxCards, isRewardFirstShow
    };
    cmdParser.addOptions(cmdParserOpts);
    cmdParser.process(*qApp);
#else
#define showSplashOrLog(str) showSplashMessage(QSplashScreen::tr(str))
    bool noGui = argc > 1 && strcmp(argv[1], "-server") == 0;

    if (noGui)
        new QCoreApplication(argc, argv);
    else
        new QApplication(argc, argv);

#if defined(Q_OS_MAC) || defined(Q_OS_ANDROID)
#define showSplashMessage(message)
#define SPLASH_DISABLED
#else
    QSplashScreen *splash = NULL;
    if (!noGui) {
        QPixmap raraLogo;
        QDate currentDate = QDate::currentDate();
        if (currentDate.month() == 11 && currentDate.day() == 30)
            raraLogo.load("image/system/developers/logo_rara.png");
        else
            raraLogo.load("image/system/developers/logo.png");

        splash = new QSplashScreen(raraLogo);
        splash->show();
        qApp->processEvents();
    }
#define showSplashMessage(message) \
    if (splash == NULL) {\
        puts(message.toUtf8().constData());\
        } else {\
        splash->showMessage(message, Qt::AlignBottom | Qt::AlignHCenter, Qt::cyan);\
        qApp->processEvents();\
        }
#endif  // Q_OS_MAC || Q_OS_ANDROID
#endif  // SRV_ONLY

#ifdef USE_BREAKPAD
    showSplashMessage(QSplashScreen::tr("Loading BreakPad..."));
    ExceptionHandler eh(L"./dmp", NULL, callback, NULL, ExceptionHandler::HANDLER_ALL);
#endif

#if defined(Q_OS_MAC) && defined(QT_NO_DEBUG)
    showSplashMessage(QSplashScreen::tr("Setting game path..."));
    QDir::setCurrent(qApp->applicationDirPath());
#endif

#ifdef Q_OS_LINUX
    showSplashOrLog("Checking game path...");
    QDir dir(QString("lua"));
    if (dir.exists() && (dir.exists(QString("config.lua")))) {
        // things look good and use current dir
    } else {
        showSplashOrLog("Setting game path...");
#ifndef Q_OS_ANDROID
        QDir::setCurrent(qApp->applicationFilePath().replace("games", "share"));
#else
        bool found = false;
        QDir storageDir("/storage");
        QStringList sdcards = storageDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (const QString &sdcard, sdcards) {
            QDir root(QString("/storage/%1/Android/data/org.qsgsrara.qsanguosha").arg(sdcard));
            if (root.exists("lua/config.lua")) {
                QDir::setCurrent(root.absolutePath());
                found = true;
                break;
            }
        }
        if (!found) {
            QDir root("/sdcard/Android/data/org.qsgsrara.qsanguosha");
            if (root.exists("lua/config.lua")) {
                QDir::setCurrent(root.absolutePath());
                found = true;
            }
        }


        if (!found) {
            QString m = QObject::tr("Game data not found, please download QSanguosha-For-Hegemony PC version, and put the files and folders into /sdcard/Android/data/org.qsgsrara.qsanguosha");
            if (!noGui)
                QMessageBox::critical(NULL, QObject::tr("Error"), m);
            else
                puts(m.toLatin1().constData());

            return -2;
        }
#endif
    }
#endif

    // initialize random seed for later use
    qsrand((unsigned int)QTime(0, 0, 0).secsTo(QTime::currentTime()));

    // load the main translation file first for we need to translate messages of splash.
    QTranslator translator;
    translator.load("sanguosha.qm");
    qApp->installTranslator(&translator);

    showSplashOrLog("Loading translation...");
    QTranslator qt_translator;
    qt_translator.load("qt_zh_CN.qm");
    qApp->installTranslator(&qt_translator);

    showSplashOrLog("Initializing game engine...");
    new Settings;
    Sanguosha = new Engine;

    showSplashOrLog("Loading user's configurations...");
    Config.init();
#ifndef SRV_ONLY
    if (!noGui) {
        QFont f = Config.AppFont;
#ifdef Q_OS_ANDROID
        f.setPointSize(12);
#endif
        qApp->setFont(f);
    }

    if (qApp->arguments().contains("-server")) {
#else
    if (cmdParser.isSet(serverName))
        Config.ServerName = cmdParser.value(serverName);
    if (cmdParser.isSet(serverAddr))
        Config.Address = cmdParser.value(serverAddr);
    if (cmdParser.isSet(serverPort))
        Config.ServerPort = cmdParser.value(serverPort).toUInt();
    if (cmdParser.isSet(playerTimeout)) {
        int val = cmdParser.value(playerTimeout).toInt();
        if (val <= 0) {
            puts("Error: Timeout must be at least 1 second.");
            cmdParser.showHelp(1);
        } else
            Config.OperationTimeout = val;
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
            Config.GameMode = mode;
        }
    }
    printf("Game mode is %s, timeout %d seconds.\n",
           Config.GameMode.toStdString().c_str(), Config.OperationTimeout);
    if (cmdParser.isSet(banPackages))
        Config.BanPackages = cmdParser.values(banPackages);
    printf("Banned packages are:");
    for (auto bp : Config.BanPackages)
        printf(" %s", bp.toStdString().c_str());
    puts("");
    if (cmdParser.isSet(isFixSeat))
        Config.RandomSeat = false;
    if (cmdParser.isSet(numLuckCards))
        Config.LuckCardLimitation = cmdParser.value(numLuckCards).toInt();
    if (cmdParser.isSet(numMaxCards))
        Config.MaxCards = cmdParser.value(numMaxCards).toInt();
    if (cmdParser.isSet(isRewardFirstShow))
        Config.RewardTheFirstShowingPlayer = true;
#endif  // SRV_ONLY
        Server *server = new Server(qApp);
        printf("Server is starting on port %u...\n", Config.ServerPort);

        if (server->listen())
            printf("Started successfully.\n");
        else
            printf("Started failed!\n");

        return qApp->exec();
#ifndef SRV_ONLY
    }

    showSplashMessage(QSplashScreen::tr("Loading style sheet..."));
    QFile file("style-sheet/sanguosha.qss");
    QString styleSheet;
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        styleSheet = stream.readAll();
    }

#ifdef Q_OS_WIN
    QFile winFile("style-sheet/windows-extra.qss");
    if (winFile.open(QIODevice::ReadOnly)) {
        QTextStream winStream(&winFile);
        styleSheet += winStream.readAll();
    }
#endif

    qApp->setStyleSheet(styleSheet + StyleHelper::styleSheetOfTooltip());

#ifdef AUDIO_SUPPORT
    showSplashMessage(QSplashScreen::tr("Initializing audio module..."));
    Audio::init();
#else
    if (!noGui)
        QMessageBox::warning(NULL, QMessageBox::tr("Warning"), QMessageBox::tr("Audio support is disabled when compiled"));
#endif

    showSplashMessage(QSplashScreen::tr("Loading main window..."));
    MainWindow main_window;

    Sanguosha->setParent(&main_window);
    main_window.show();
#ifndef SPLASH_DISABLED
    if (splash != NULL) {
        splash->finish(&main_window);
        delete splash;
    }
#endif

    foreach (const QString &_arg, qApp->arguments()) {
        QString arg = _arg;
        if (arg.startsWith("-connect:")) {
            arg.remove("-connect:");
            Config.HostAddress = arg;
            Config.setValue("HostAddress", arg);

            main_window.startConnection();
            break;
        }
    }

    return qApp->exec();
#endif // SRV_ONLY
}
