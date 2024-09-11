/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CALENDAR_CONTROLLER_H
#define CALENDAR_CONTROLLER_H

#include <KCalendarCore/FileStorage>
#include <KCalendarCore/MemoryCalendar>
#include <QMap>
#include <QNetworkAccessManager>
#include <QObject>
#include <QQmlEngine>
#include <QVariantMap>

class LocalCalendar;

class CalendarController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString calendars READ calendars NOTIFY calendarsChanged)

public:
    explicit CalendarController(QObject *parent = nullptr);
    void setNetworkAccessManager(QNetworkAccessManager *nam);

    QString calendars() const;
    QString calendarFile(const QString &calendarId);

    KCalendarCore::MemoryCalendar::Ptr createLocalCalendar(const QString &calendarId, const QByteArray &timeZoneId);
    KCalendarCore::MemoryCalendar::Ptr memoryCalendar(const QString &calendarId) const;
    void createCalendarFromUrl(const QString &calendarId, const QUrl &url, const QByteArray &timeZoneId);
    void deleteCalendar(const QString &calendarId);
    bool save(const QString &calendarId);
    Q_INVOKABLE QVariantMap exportData(const QString &calendarName, LocalCalendar *sourceCalendar);

Q_SIGNALS:
    void calendarsChanged();
    void calendarDownloaded(const QString &calendarId);
    void downlading(const QString &calendarId, const bool downlading);

private:
    void downloadFinished(const QString &calendarId, const QByteArray &timeZoneId, const QString &filePath);
    static QString filenameToPath(const QString &calendarId);
    void removeCalendarFromConfig(const QString &calendarId);
    bool saveToDisk(const QString &filename, const QByteArray &data);
    void loadSavedConferences();
    void addConferenceToConfig(const QString &calendarId);
    void addTzIdToConfig(const QString &calendarId, const QByteArray &timeZoneId);
    QByteArray tzIdFromConfig(const QString &calendarId) const;

    QMap<QString, KCalendarCore::FileStorage::Ptr> m_storages;
    QMap<QString, KCalendarCore::MemoryCalendar::Ptr> m_calendars;

    class Private;
    Private *const d;
};

#endif
