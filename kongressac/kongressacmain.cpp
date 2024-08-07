/*
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "calalarmclient.h"
#include <KAboutData>
#include <KCrash>
#include <KDBusService>

#include <KLocalizedString>
#include <QCommandLineParser>
#include <QGuiApplication>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    KAboutData aboutData{QStringLiteral("kongressac"),
                         i18n("Kongress Alarm Check Daemon"),
                         QString{},
                         i18n("Kongress Alarm Check Daemon"),
                         KAboutLicense::GPL,
                         i18n("(c) 2021 Dimitris Kardarakos"),
                         QString{},
                         QString{}};
    aboutData.addAuthor(i18n("Dimitris Kardarakos"), i18n("Maintainer"), QStringLiteral("dimkard@posteo.net"));

    KCrash::initialize();

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service{KDBusService::Unique};
    CalAlarmClient client;

    return app.exec();
}
