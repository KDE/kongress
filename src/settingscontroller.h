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

#ifndef SETTINGS_CONTROLLER_H
#define SETTINGS_CONTROLLER_H

#include <QObject>
#include <QQmlEngine>

class SettingsController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool displayInLocalTimezone READ displayInLocalTimezone WRITE setDisplayInLocalTimezone NOTIFY displayInLocalTimezoneChanged)

public:
    explicit SettingsController(QObject* parent = nullptr);
    ~SettingsController() override;

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

    bool displayInLocalTimezone() const;
    void setDisplayInLocalTimezone(bool displayInLocalTimezone);

Q_SIGNALS:
    void displayInLocalTimezoneChanged();

private:
    class Private;
    Private* d;
};

#endif
