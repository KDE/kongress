/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CONFERENCEMODEL_H
#define CONFERENCEMODEL_H

#include <QAbstractListModel>
#include <QVariantMap>
#include <QVector>
#include <QHash>

class Conference;
class ConferenceController;

class ConferenceModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(ConferenceController *controller READ controller WRITE setController NOTIFY controllerChanged)
    Q_PROPERTY(bool busyDownlading READ busyDownlading NOTIFY busyDownladingChanged)

public:
    enum Roles {
        ConferenceId = Qt::UserRole + 1,
        ConferenceName,
        ConferenceDescription,
        ConferenceIcalUrl,
        ConferenceDays,
        ConferenceVenueImageUrl,
        ConferenceVenueLongitude,
        ConferenceVenueLatitude,
        ConferenceVenueOsmUrl,
        ConferenceTimeZone,
        PastUpcoming
    };

    explicit ConferenceModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    ConferenceController *controller() const;
    void setController(ConferenceController *conferenceController);

    bool busyDownlading() const;

Q_SIGNALS:
    void controllerChanged();
    void busyDownladingChanged();

private Q_SLOTS:
    void setBusyStatus(const bool downlading);

private:
    void loadConferences();
    QString pastOrUpcoming(const int index) const;

    ConferenceController *m_controller;
    QVector<Conference *> m_conferences;
    bool m_busy_downloading;

};
#endif
