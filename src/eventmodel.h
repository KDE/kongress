/*
 * Copyright (C) 2018-2020 Dimitris Kardarakos
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

#ifndef EVENTMODEL_H
#define EVENTMODEL_H

#include <QAbstractListModel>
#include <KCalendarCore/Event>

using namespace KCalendarCore;

class LocalCalendar;

class EventModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QDate filterdt READ filterdt WRITE setFilterdt NOTIFY filterdtChanged)
    Q_PROPERTY(LocalCalendar* calendar READ calendar WRITE setCalendar NOTIFY calendarChanged)
    Q_PROPERTY(QString eventCategory READ eventCategory WRITE setEventCategory NOTIFY eventCategoryChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY rowCountChanged)

public:
    explicit EventModel(QObject* parent = nullptr);
    ~EventModel() override;

    enum Roles
    {
        Uid = Qt::UserRole+1,
        LastModified,
        ScheduleStartDt,
        EventStartDt,
        AllDay,
        Description,
        Summary,
        Location,
        Categories,
        Priority,
        Created,
        Secrecy,
        ScheduleEndDt,
        EventEndDt,
        Transparency,
        IsRepeating,
        RepeatPeriodType,
        RepeatEvery,
        RepeatStopAfter,
        EventCategories,
        Url,
        ShiftedStartEndDt,
        ShiftedStartEndTime,
        StartEndDt,
        Overlapping
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QDate filterdt() const;
    void setFilterdt(const QDate & filterDate);

    LocalCalendar* calendar() const;
    void setCalendar(LocalCalendar* const calendarPtr);

    QString eventCategory() const;
    void setEventCategory(const QString & category);

public Q_SLOTS:
    void loadEvents();

Q_SIGNALS:
    void filterdtChanged();
    void calendarChanged();
    void eventCategoryChanged();
    void rowCountChanged();

private:
    /**
     * @return The INTERVAL of RFC 5545. It contains a positive integer representing at
      which intervals the recurrence rule repeats.
     */
    int repeatEvery(const int idx) const;

    /**
     * @return The COUNT of RFC 5545. It defines the number of occurrences at which to
      range-bound the recurrence.  The "DTSTART" property value always
      counts as the first occurrence.
     */
    int repeatStopAfter(const int idx) const;

    /**
     * @return The FREQ rule part which identifies the type of recurrence rule
     */
    ushort repeatPeriodType(const int idx) const;

    /**
     * @return The number of events that overlap with a specific event
     */
    int overlappingEvents(const int idx) const;

    Event::List m_events;
    QDate m_filterdt;
    QString m_category;
    LocalCalendar* m_local_calendar;
};

#endif //EVENTMODEL_H
