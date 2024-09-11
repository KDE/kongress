/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EVENTMODEL_H
#define EVENTMODEL_H

#include "localcalendar.h"

#include <KCalendarCore/Event>
#include <QAbstractListModel>

class SettingsController;

class EventModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QDate filterdt READ filterdt WRITE setFilterdt NOTIFY filterdtChanged)
    Q_PROPERTY(LocalCalendar *calendar READ calendar WRITE setCalendar NOTIFY calendarChanged)
    Q_PROPERTY(QString eventCategory READ eventCategory WRITE setEventCategory NOTIFY eventCategoryChanged)

public:
    explicit EventModel(QObject *parent = nullptr);

    enum Roles {
        Uid = Qt::UserRole + 1,
        LastModified,
        ScheduleStartDt,
        EventStartDt,
        EventDt,
        ShiftedEventDt,
        ShiftedEventDtLocal,
        Description,
        Summary,
        Location,
        Categories,
        ScheduleEndDt,
        EventEndDt,
        EventCategories,
        Url,
        ShiftedStartEndDt,
        ShiftedStartEndDtLocal,
        ShiftedStartEndTime,
        ShiftedStartEndTimeLocal,
        StartEndDt,
        StartEndDtLocal,
        Overlapping,
        ConferenceTzId
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QDate filterdt() const;
    void setFilterdt(const QDate &filterDate);

    LocalCalendar *calendar() const;
    void setCalendar(LocalCalendar *const calendarPtr);

    QString eventCategory() const;
    void setEventCategory(const QString &category);

public Q_SLOTS:
    void loadEvents();

Q_SIGNALS:
    void filterdtChanged();
    void calendarChanged();
    void eventCategoryChanged();

private:
    /**
     * @return The number of events that overlap with a specific event
     */
    int overlappingEvents(const int idx) const;

    QString formatStartEndTime(const QDateTime &startDtTime, const QDateTime &endDtTime) const;
    QString formatStartEndDt(const QDateTime &startDtTime, const QDateTime &endDtTime, bool allDay) const;

    KCalendarCore::Event::List m_events;
    QDate m_filterdt;
    QString m_category;
    LocalCalendar *m_local_calendar = nullptr;
    SettingsController *const m_settings_controller;
};

#endif // EVENTMODEL_H
