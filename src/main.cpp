/*
 *   Copyright 2018-2020 Dimitris Kardarakos <dimkard@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>
#include <QQuickStyle>
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

    KAboutData about {QStringLiteral("kongress"), i18n("Kongress"), QStringLiteral("0.1"), i18n("KDE Conference Companion"), KAboutLicense::GPL, i18n("© 2020 KDE Community")};
    about.addAuthor(i18n("Dimitris Kardarakos"), QString {}, QStringLiteral("dimkard@posteo.net"));
    KAboutData::setApplicationData(about);

    qmlRegisterType<LocalCalendar>("org.kde.kongress", 0, 1, "LocalCalendar");
    qmlRegisterType<EventModel>("org.kde.kongress", 0, 1, "EventModel");
    qmlRegisterType<EventController>("org.kde.kongress", 0, 1, "EventController");
    qmlRegisterType<ConferenceModel>("org.kde.kongress", 0, 1, "ConferenceModel");
    qmlRegisterType<CalendarController>("org.kde.kongress", 0, 1, "CalendarController");
    qmlRegisterType<ConferenceController>("org.kde.kongress", 0, 1, "ConferenceController");
    qmlRegisterType<Conference>("org.kde.kongress", 0, 1, "Conference");

    qmlRegisterSingletonType<SettingsController>("org.kde.kongress", 0, 1, "SettingsController", &SettingsController::qmlInstance);

#ifdef Q_OS_ANDROID
    QQuickStyle::setStyle(QStringLiteral("Material"));
#endif

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext {&engine});

    ConferenceController conferenceController;
    engine.rootContext()->setContextProperty(QStringLiteral("_conferenceController"), &conferenceController); //TODO: Make it singleton

    CalendarController calendarController;
    engine.rootContext()->setContextProperty(QStringLiteral("_calendarController"), &calendarController); //TODO: Make it singleton

    EventController eventController;
    eventController.setCalendarController(&calendarController);

    engine.rootContext()->setContextProperty(QStringLiteral("_eventController"), &eventController);  //TODO: Make it singleton

    engine.load(QUrl {QStringLiteral("qrc:///Main.qml")});

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    int ret = app.exec();
    return ret;
}
