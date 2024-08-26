/*
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QtGlobal>

#ifdef Q_OS_ANDROID
#include <KColorSchemeManager>
#include <QGuiApplication>
#else
#include <QApplication>

#include <KCrash>
#endif

#include "calendarcontroller.h"
#include "conference.h"
#include "conferencecontroller.h"
#include "conferencemodel.h"
#include "eventcontroller.h"
#include "eventmodel.h"
#include "localcalendar.h"
#include "settingscontroller.h"
#include "version.h"

#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>

#include <QCommandLineParser>
#include <QIcon>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QUrl>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    QGuiApplication app{argc, argv};
    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
    KColorSchemeManager colorMgr; // enables automatic dark mode handling
#else
    QApplication app{argc, argv};
    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
#endif
    KLocalizedString::setApplicationDomain("kongress");

    KAboutData about{QStringLiteral("kongress"),
                     i18n("Kongress"),
                     QStringLiteral(KONGRESS_VERSION_STRING),
                     i18n("KDE Conference Companion"),
                     KAboutLicense::GPL_V3,
                     i18n("© 2021 KDE Community")};
    about.setOrganizationDomain(QByteArray{"kde.org"});
    about.setProductName(QByteArray{"kongress"});
    about.addAuthor(i18nc("@info:credit", "Dimitris Kardarakos"), i18nc("@info:credit", "Maintainer and Developer"), QStringLiteral("dimkard@posteo.net"));
    about.setHomepage(QStringLiteral("https://invent.kde.org/utilities/kongress"));

    KAboutData::setApplicationData(about);

#ifndef Q_OS_ANDROID
    KCrash::initialize();
#endif

    QCommandLineParser parser;
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.kongress")));

    QNetworkAccessManager nam;
    nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    nam.enableStrictTransportSecurityStore(true, QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1String("/hsts/"));
    nam.setStrictTransportSecurityEnabled(true);
    QNetworkDiskCache namDiskCache;
    namDiskCache.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1String("/nam/"));
    nam.setCache(&namDiskCache);

    qmlRegisterType<LocalCalendar>("org.kde.kongress", 0, 1, "LocalCalendar");
    qmlRegisterType<EventModel>("org.kde.kongress", 0, 1, "EventModel");
    qmlRegisterType<EventController>("org.kde.kongress", 0, 1, "EventController");
    qmlRegisterType<ConferenceModel>("org.kde.kongress", 0, 1, "ConferenceModel");
    qmlRegisterType<Conference>("org.kde.kongress", 0, 1, "Conference");

    qmlRegisterSingletonType<SettingsController>("org.kde.kongress", 0, 1, "SettingsController", &SettingsController::qmlInstance);

    ConferenceController conferenceController;
    conferenceController.setNetworkAccessManager(&nam);
    qmlRegisterSingletonInstance<ConferenceController>("org.kde.kongress", 0, 1, "ConferenceController", &conferenceController);

    CalendarController calendarController;
    calendarController.setNetworkAccessManager(&nam);
    qmlRegisterSingletonInstance<CalendarController>("org.kde.kongress", 0, 1, "CalendarController", &calendarController);

    EventController eventController;
    eventController.setCalendarController(&calendarController);
    qmlRegisterSingletonInstance<EventController>("org.kde.kongress", 0, 1, "EventController", &eventController);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext{&engine});
    engine.loadFromModule("org.kde.kongress", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    int ret = app.exec();
    return ret;
}
