/*
 * Copyright (C) 2020 David Barchiesi <david@barchie.si>
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

#include "settingscontroller.h"

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
    : QObject(parent), d(new Private)
{
}

SettingsController::~SettingsController()
{
    delete d;
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

void SettingsController::setDisplayInLocalTimezone(bool displayInLocalTimezone)
{
    d->config.group("general").writeEntry("displayInLocalTimezone", displayInLocalTimezone);
    d->config.sync();

    Q_EMIT displayInLocalTimezoneChanged();
}
