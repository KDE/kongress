/*
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>
#include <QQuickStyle>
#include <QIcon>
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

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app {argc, argv};
    KLocalizedString::setApplicationDomain("kongress");

    KAboutData about {QStringLiteral("kongress"), i18n("Kongress"), QStringLiteral("1.0.1"), i18n("KDE Conference Companion"), KAboutLicense::GPL_V3, i18n("© 2021 KDE Community")};
    about.setOrganizationDomain(QByteArray {"kde.org"});
    about.setProductName(QByteArray {"kongress"});
    about.addAuthor(i18nc("@info:credit", "Dimitris Kardarakos"), i18nc("@info:credit", "Maintainer and Developer"), QStringLiteral("dimkard@posteo.net"));
    about.setHomepage(QStringLiteral("https://invent.kde.org/utilities/kongress"));

    KAboutData::setApplicationData(about);

    QCommandLineParser parser;
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    QApplication::setApplicationName(about.componentName());
    QApplication::setApplicationDisplayName(about.displayName());
    QApplication::setOrganizationDomain(about.organizationDomain());
    QApplication::setApplicationVersion(about.version());
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.kongress")));

    qmlRegisterType<LocalCalendar>("org.kde.kongress", 0, 1, "LocalCalendar");
    qmlRegisterType<EventModel>("org.kde.kongress", 0, 1, "EventModel");
    qmlRegisterType<EventController>("org.kde.kongress", 0, 1, "EventController");
    qmlRegisterType<ConferenceModel>("org.kde.kongress", 0, 1, "ConferenceModel");
    qmlRegisterType<Conference>("org.kde.kongress", 0, 1, "Conference");

    qmlRegisterSingletonType<SettingsController>("org.kde.kongress", 0, 1, "SettingsController", &SettingsController::qmlInstance);
    qmlRegisterSingletonType<ConferenceController>("org.kde.kongress", 0, 1, "ConferenceController", &ConferenceController::qmlInstance);;

    static CalendarController *s_calendar_controller {nullptr};
    static EventController *s_event_controller {nullptr};

    qmlRegisterSingletonType<EventController>("org.kde.kongress", 0, 1, "EventController", [](QQmlEngine * engine, QJSEngine *) -> QObject * {
        engine->setObjectOwnership(s_event_controller, QQmlEngine::CppOwnership);
        return s_event_controller;
    });

    qmlRegisterSingletonType<CalendarController>("org.kde.kongress", 0, 1, "CalendarController", [](QQmlEngine * engine, QJSEngine *) -> QObject * {
        engine->setObjectOwnership(s_calendar_controller, QQmlEngine::CppOwnership);
        return s_calendar_controller;
    });

    CalendarController calendarController;
    s_calendar_controller = &calendarController;

    EventController eventController;
    eventController.setCalendarController(&calendarController);
    s_event_controller = &eventController;

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
