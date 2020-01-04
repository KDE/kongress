/*
 * Copyright (C) 2019 Dimitris Kardarakos
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

#include "kdefosdemconfig.h"

#include <KLocalizedString>
#include <KConfig>
#include <KConfigGroup>
#include <QDebug>
#include <QRegExp>
#include <QDir>

class KDEFosdemConfig::Private
{
public:
    Private()
        : config("kdefosdemrc")
    {};
    KConfig config;
};

KDEFosdemConfig::KDEFosdemConfig(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
    QString calendars = d->config.group("general").readEntry("calendars", QString());
    if(calendars.isEmpty()) {
        qDebug() << "No calendar found, creating a default one";
        addCalendar("personal");
        setActiveCalendar("personal");
        d->config.sync();
    }
}

KDEFosdemConfig::~KDEFosdemConfig()
{
    delete d;
}

QString KDEFosdemConfig::calendars() const
{
   return d->config.group("general").readEntry("calendars", QString());
}

QString KDEFosdemConfig::activeCalendar() const
{
    return d->config.group("general").readEntry("activeCalendar", QString());
}


void KDEFosdemConfig::setActiveCalendar(const QString & calendar)
{
    d->config.group("general").writeEntry("activeCalendar", calendar);
    d->config.sync();
    emit activeCalendarChanged();
}

QVariantMap KDEFosdemConfig::canAddCalendar(const QString& calendar)
{
    QVariantMap result;
    result["success"] = QVariant(true);
    result["reason"] = QVariant(QString());

    QRegExp invalidChars("[\\;\\\\/<>:\\?\\*|\"\']");
    if(calendar.contains(invalidChars))
    {
        result["success"] = QVariant(false);
        result["reason"] = QVariant(i18n("Calendar name contains invalid characters"));
        return result;
    }

    if(d->config.group("general").readEntry("calendars", QString()).isEmpty())
    {
        return result;
    }

    QStringList calendarsList = d->config.group("general").readEntry("calendars", QString()).split(";");

    if(calendarsList.contains(calendar))
    {
        result["success"] = QVariant(false);
        result["reason"] = QVariant(i18n("Calendar already exists"));
        return result;
    }

    return result;
}

QVariantMap KDEFosdemConfig::addCalendar(const QString & calendar)
{
    QVariantMap result;
    result["success"] = QVariant(true);
    result["reason"] = QVariant(QString());

    QVariantMap canAddResult = canAddCalendar(calendar);

    if(!(canAddResult["success"].toBool()))
    {
        result["success"] = QVariant(false);
        result["reason"] = QVariant(canAddResult["reason"].toString());
        return result;
    }

    if(d->config.group("general").readEntry("calendars", QString()).isEmpty())
    {
        d->config.group("general").writeEntry("calendars", calendar);
        d->config.sync();

        return result;
    }

    QStringList calendarsList = d->config.group("general").readEntry("calendars", QString()).split(";");
    calendarsList.append(calendar);
    d->config.group("general").writeEntry("calendars", calendarsList.join(";"));
    d->config.sync();

    emit calendarsChanged();

    return result;
}

void KDEFosdemConfig::removeCalendar(const QString& calendar)
{
    d->config.reparseConfiguration();
    QStringList calendarsList = d->config.group("general").readEntry("calendars", QString()).split(";");
    if(calendarsList.contains(calendar))
    {
        qDebug() << "Removing calendar " << calendar;
        calendarsList.removeAll(calendar);

        d->config.deleteGroup(calendar);
        d->config.group("general").writeEntry("calendars", calendarsList.join(";"));
        d->config.sync();

        emit calendarsChanged();
    }
}

QString KDEFosdemConfig::calendarFile(const QString& calendarName)
{
    if(d->config.hasGroup(calendarName) && d->config.group(calendarName).hasKey("file"))
    {
        return  d->config.group(calendarName).readEntry("file");
    }
    d->config.group(calendarName).writeEntry("file", filenameToPath(calendarName));
    d->config.sync();

    return filenameToPath(calendarName);
}

QString KDEFosdemConfig::filenameToPath(const QString& calendarName)
{
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir baseFolder(basePath);
    baseFolder.mkpath(QStringLiteral("."));

    return basePath + "/kdefosdem_" + calendarName + ".ics";
}
