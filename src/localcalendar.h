/*
 * Copyright (C) 2018 Dimitris Kardarakos
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LOCALCALENDAR_H
#define LOCALCALENDAR_H

#include <QSharedPointer>
#include <KCalendarCore/MemoryCalendar>
#include <KCalendarCore/FileStorage>
#include <KCalendarCore/Event>
#include <QVariantMap>
#include <QtNetwork>

using namespace KCalendarCore;

class LocalCalendar : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap calendarInfo READ calendarInfo WRITE setCalendarInfo NOTIFY calendarInfoChanged)
    Q_PROPERTY(QSharedPointer<MemoryCalendar> memorycalendar READ memorycalendar NOTIFY memorycalendarChanged)
    Q_PROPERTY(QSharedPointer<FileStorage> calendarstorage READ calendarstorage WRITE setCalendarstorage NOTIFY calendarstorageChanged) //TODO: Separate storage from LocalCalendar, move storage logic to CalendarController
    Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged);

public:
    explicit LocalCalendar(QObject* parent = nullptr);
    ~LocalCalendar() override;

    MemoryCalendar::Ptr memorycalendar() const;
    FileStorage::Ptr calendarstorage() const; //TODO: Storage logic to CalendarController
    QVariantMap calendarInfo() const;
    QStringList categories() const;

    void setCalendarstorage(FileStorage::Ptr calendarStorage); //TODO: Storage logic to CalendarController
    bool saveToDisk(const QString& filename, QIODevice *data); //TODO: Storage logic to CalendarController
    void setCalendarInfo(const QVariantMap& calendarInfoMap);

public Q_SLOTS:
    void deleteCalendar(); //TODO: Storage logic to CalendarController
    bool save(); //TODO: Storage logic to CalendarController
    static QVariantMap importCalendar(const QString& calendarName, const QString& sourcePath); //TODO: Storage logic to CalendarController
    void downloadFinished(QNetworkReply *reply); //TODO: Storage logic to CalendarController
Q_SIGNALS:
    void memorycalendarChanged();
    void calendarstorageChanged(); //TODO: Storage logic to CalendarController
    void calendarInfoChanged();
    void categoriesChanged();

private:
    static QVariantMap canCreateFile(const QString& calendarName); //TODO: Storage logic to CalendarController
    void createLocalCalendar(const QString& calendarName); //TODO: Storage logic to CalendarController

    MemoryCalendar::Ptr m_calendar;
    FileStorage::Ptr m_cal_storage; //TODO: Storage logic to CalendarController
    QString m_fullpath;  //TODO: Storage logic to CalendarController
    QNetworkAccessManager m_DownloadManager; //TODO: Storage logic to CalendarController
    QVariantMap m_calendarInfo;


};

#endif // LOCALCALENDAR_H

