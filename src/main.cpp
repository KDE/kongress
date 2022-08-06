/*
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QtGlobal>

#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>
#include <QQuickStyle>
#include <QIcon>
#include <QStandardPaths>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KAboutData>
#include "calendarcontroller.h"
#include "localcalendar.h"
#include "eventmodel.h"
#include "eventcontroller.h"
#include "conferencemodel.h"
#include "conferencecontroller.h"
#include "settingscontroller.h"
#include "calendarcontroller.h"
#include "conference.h"
#include "version.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#ifdef Q_OS_ANDROID
    QGuiApplication app {argc, argv};
#else
    QApplication app {argc, argv};
#endif
    KLocalizedString::setApplicationDomain("kongress");

    KAboutData about {QStringLiteral("kongress"), i18n("Kongress"), QStringLiteral(KONGRESS_VERSION_STRING), i18n("KDE Conference Companion"), KAboutLicense::GPL_V3, i18n("© 2021 KDE Community")};
    about.setOrganizationDomain(QByteArray {"kde.org"});
    about.setProductName(QByteArray {"kongress"});
    about.addAuthor(i18nc("@info:credit", "Dimitris Kardarakos"), i18nc("@info:credit", "Maintainer and Developer"), QStringLiteral("dimkard@posteo.net"));
    about.setHomepage(QStringLiteral("https://invent.kde.org/utilities/kongress"));

    KAboutData::setApplicationData(about);

    QCommandLineParser parser;
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    QGuiApplication::setApplicationName(about.componentName());
    QGuiApplication::setApplicationDisplayName(about.displayName());
    QGuiApplication::setOrganizationDomain(about.organizationDomain());
    QGuiApplication::setApplicationVersion(about.version());
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

#ifdef Q_OS_ANDROID
    QQuickStyle::setStyle(QStringLiteral("Material"));
#endif

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext {&engine});
    engine.rootContext()->setContextProperty(QStringLiteral("_about"), QVariant::fromValue(about));
    engine.load(QUrl {QStringLiteral("qrc:///Main.qml")});

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    int ret = app.exec();
    return ret;
}
