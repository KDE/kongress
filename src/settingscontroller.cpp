/*
 * SPDX-FileCopyrightText: 2020 David Barchiesi <david@barchie.si>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settingscontroller.h"
#include "alarmchecker.h"
#include <KConfig>
#include <KConfigGroup>

class SettingsController::Private
{
public:
    Private()
        : config("kongressrc")
    {};
    KConfig config;
};

SettingsController::SettingsController(QObject *parent)
    : QObject {parent}, d {new Private}, m_alarm_checker {new AlarmChecker {this}}
{
}

QObject *SettingsController::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new SettingsController;
}

bool SettingsController::displayInLocalTimezone() const
{
    return d->config.group("general").readEntry("displayInLocalTimezone", false);
}

void SettingsController::setDisplayInLocalTimezone(const bool displayInLocalTimezone)
{
    d->config.group("general").writeEntry("displayInLocalTimezone", displayInLocalTimezone);
    d->config.sync();

    Q_EMIT displayInLocalTimezoneChanged();
}

bool SettingsController::remindFavorites() const
{
    return d->config.group("notifications").readEntry("remindFavorites", true);
}

void SettingsController::setRemindFavorites(const bool remind)
{
    d->config.group("notifications").writeEntry("remindFavorites", remind);
    d->config.sync();

    Q_EMIT remindFavoritesChanged();
}

int SettingsController::remindBeforeStart() const
{
    return d->config.group("notifications").readEntry("remindBeforeStart", 5);
}

void SettingsController::setRemindBeforeStart(const int remindBeforeStart)
{
    d->config.group("notifications").writeEntry("remindBeforeStart", remindBeforeStart);
    d->config.sync();

    Q_EMIT remindBeforeStartChanged();
}

bool SettingsController::canAddReminder() const
{
    if (m_alarm_checker != nullptr) {
        return m_alarm_checker->active();
    }

    return false;
}

#include "moc_settingscontroller.cpp"
