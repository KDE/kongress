/*
 *   Copyright 2020 Dimitris Kardarakos <dimkard@gmail.com>
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
#include <KLocalizedContext>
#include "calendarcontroller.h"
#include "localcalendar.h"
#include "eventmodel.h"
#include "eventcontroller.h"
#include "incidencealarmsmodel.h"
#include "conferencemodel.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationDomain("kde.org");
    QCoreApplication::setApplicationName("kongress");

    qmlRegisterType<LocalCalendar>("org.kde.kongress", 0, 1, "LocalCalendar");
    qmlRegisterType<EventModel>("org.kde.kongress", 0, 1, "EventModel");
    qmlRegisterType<EventController>("org.kde.kongress",0,1,"EventController");
    qmlRegisterType<IncidenceAlarmsModel>("org.kde.kongress",0,1,"IncidenceAlarmsModel");
    qmlRegisterType<ConferenceModel>("org.kde.kongress",0,1,"ConferenceModel");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    engine.load(QUrl(QStringLiteral("qrc:///Main.qml")));

    EventController eventController;
    engine.rootContext()->setContextProperty(QStringLiteral("_eventController"), &eventController);

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    int ret = app.exec();
    return ret;


}
