/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "alarmsmodel.h"
#include <KCalendarCore/Calendar>
#include <KCalendarCore/MemoryCalendar>
#include <KSharedConfig>
#include <QDebug>

AlarmsModel::AlarmsModel(QObject *parent)
    : QObject{parent}
    , m_calendars{QVector<KCalendarCore::Calendar::Ptr>{}}
    , m_file_storages{QVector<KCalendarCore::FileStorage::Ptr>{}}
    , m_alarms{KCalendarCore::Alarm::List{}}
    , m_calendar_files{QStringList()}
{
    connect(this, &AlarmsModel::periodChanged, this, &AlarmsModel::loadAlarms);
    connect(this, &AlarmsModel::calendarsChanged, this, &AlarmsModel::loadAlarms);
}

void AlarmsModel::loadAlarms()
{
    m_alarms.clear();

    if (!(m_period.from.isValid()) && !(m_period.to.isValid())) {
        return;
    }

    openLoadStorages();

    for (const auto &m : std::as_const(m_calendars)) {
        KCalendarCore::Alarm::List calendarAlarms;

        if (m_period.from.isValid() && m_period.to.isValid()) {
            calendarAlarms = m->alarms(m_period.from, m_period.to, true);
        } else if (!(m_period.from.isValid()) && m_period.to.isValid()) {
            calendarAlarms = m->alarms(QDateTime{QDate{1900, 1, 1}, QTime{0, 0, 0}}, m_period.to);
        }

        if (!(calendarAlarms.empty())) {
            m_alarms.append(calendarAlarms);
        }
    }
    qDebug() << "loadAlarms:" << m_period.from.toString(u"dd.MM.yyyy hh:mm:ss") << "to" << m_period.to.toString(u"dd.MM.yyyy hh:mm:ss") << m_alarms.count()
             << "alarms found";

    closeStorages();
}

void AlarmsModel::setCalendars()
{
    m_file_storages.clear();
    m_calendars.clear();

    for (const auto &cf : std::as_const(m_calendar_files)) {
        KCalendarCore::Calendar::Ptr calendar{new KCalendarCore::MemoryCalendar{QTimeZone::systemTimeZoneId()}};
        KCalendarCore::FileStorage::Ptr storage{new KCalendarCore::FileStorage{calendar}};
        storage->setFileName(cf);
        if (!(storage->fileName().isNull())) {
            m_file_storages.append(storage);
            m_calendars.append(calendar);
        }
    }

    Q_EMIT calendarsChanged();
}

void AlarmsModel::openLoadStorages()
{
    auto loaded{true};
    for (const auto &fs : std::as_const(m_file_storages)) {
        loaded = fs->open() && fs->load() && loaded;
    }
}

void AlarmsModel::closeStorages()
{
    auto closed{true};
    for (const auto &fs : std::as_const(m_file_storages)) {
        closed = fs->close() && closed;
    }
}

QDateTime AlarmsModel::parentStartDt(const int idx) const
{
    auto alarm = m_alarms.at(idx);
    KCalendarCore::Duration offsetDuration;
    auto alarmTime = m_alarms.at(idx)->time();
    if (alarm->hasStartOffset()) {
        offsetDuration = alarm->startOffset();
    }

    if (!(offsetDuration.isNull())) {
        auto secondsFromStart = offsetDuration.asSeconds();

        return alarmTime.addSecs(-1 * secondsFromStart);
    }

    return alarmTime;
}

KCalendarCore::Alarm::List AlarmsModel::alarms() const
{
    return m_alarms;
}

FilterPeriod AlarmsModel::period() const
{
    return m_period;
}

void AlarmsModel::setPeriod(const FilterPeriod &filterPeriod)
{
    m_period = filterPeriod;

    Q_EMIT periodChanged();
}

QStringList AlarmsModel::calendarFiles() const
{
    return m_calendar_files;
}

void AlarmsModel::setCalendarFiles(const QStringList &fileList)
{
    m_calendar_files = fileList;

    setCalendars();
}

QDateTime AlarmsModel::firstAlarmTime() const
{
    auto firstAlarmTime = m_period.to;

    for (const auto &alarm : m_alarms) {
        auto alarmTime = alarm->time();
        if (alarmTime < firstAlarmTime) {
            firstAlarmTime = alarmTime;
        }
    }

    return firstAlarmTime;
}

#include "moc_alarmsmodel.cpp"
