/*
 * Copyright (C) 2019-2020 Dimitris Kardarakos
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

#ifndef CALENDAR_CONTROLLER_H
#define CALENDAR_CONTROLLER_H

#include <QObject>
#include <QVariantMap>
#include <QMap>
#include <QtNetwork>
#include <KCalendarCore/FileStorage>
#include <KCalendarCore/MemoryCalendar>

struct DonwloadManager {
    QString calendarId;
    QByteArray calendarTzId;
    QNetworkAccessManager networkManager;
};

class CalendarController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString calendars READ calendars NOTIFY calendarsChanged)

public:

    explicit CalendarController(QObject *parent = nullptr);
    ~CalendarController() override;

    QString calendars() const;
    QString calendarFile(const QString &calendarId);

    KCalendarCore::MemoryCalendar::Ptr createLocalCalendar(const QString &calendarId, const QByteArray &timeZoneId);
    KCalendarCore::MemoryCalendar::Ptr memoryCalendar(const QString &calendarId) const;
    void createCalendarFromUrl(const QString &calendarId, const QUrl &url, const QByteArray &timeZoneId);
    QVariantMap importCalendar(const QString &calendarId, const QString &sourcePath);
    void deleteCalendar(const QString &calendarId);
    bool save(const QString &calendarId);

Q_SIGNALS:
    void calendarsChanged();
    void calendarDownloaded(const QString &calendarId);

public Q_SLOTS:
    QVariantMap canAddCalendar(const QString &calendar);
    QVariantMap addCalendar(const QString &calendar);
    void downloadFinished(QNetworkReply *reply);

private:
    static QString filenameToPath(const QString &calendarId);
    QVariantMap canCreateFile(const QString &calendarId);
    void removeCalendarFromConfig(const QString &calendarId);
    bool saveToDisk(const QString &filename, QIODevice *data);
    void loadSavedConferences();
    void addConferenceToConfig(const QString &calendarId);
    void addTzIdToConfig(const QString &calendarId, const QByteArray &timeZoneId);
    QByteArray tzIdFromConfig(const QString &calendarId) const;

    QMap<QString, KCalendarCore::FileStorage::Ptr> m_storages;
    QMap<QString, KCalendarCore::MemoryCalendar::Ptr> m_calendars;
    DonwloadManager *m_downloadManager;

    class Private;
    Private *d;
};

#endif
