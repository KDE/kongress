/*
 *   Copyright 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.7
import QtQuick.Controls 2.4 as Controls2
import QtQuick.Layouts 1.11
import org.kde.kirigami 2.7 as Kirigami
import org.kde.phone.kdefosdem 0.1 as KDEFOSDEM

Kirigami.Page {
    id: root

    property string viewMode
    property date eventStartDt
    property var roCalendar
    property var rwCalendar
    property string category

    signal eventsUpdated

    function reload()
    {
        cardsListview.model.loadEvents();
    }

    Controls2.Label {
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        visible: eventsModel.count == 0
        wrapMode: Text.WordWrap
        text: eventStartDt.toLocaleDateString() != "" ? i18n("No events found for %1", eventStartDt.toLocaleDateString(Qt.locale(), Locale.ShortFormat)) : i18n("No events found")
        font.pointSize: Kirigami.Units.fontMetrics.font.pointSize * 1.5
    }

    Component {
        id: eventInfo

        EventInfo {
            rwCalendar: root.rwCalendar

            onEventUpdated: root.eventsUpdated()
        }
    }

    ListView {
        id: cardsListview

        anchors.fill: parent

        model: eventsModel

        delegate: Kirigami.SwipeListItem {
            id: itemDelegate

            actions: [
                Kirigami.Action {
                    text: i18n("More Info")
                    icon.name: "documentinfo"

                    onTriggered: pageStack.push(eventInfo, {event: model})
                },

                Kirigami.Action {
                    text: viewMode == "favorites" ? i18n("Delete") : i18n("Add to Favorites")
                    icon.name: viewMode == "favorites" ? "delete" : "favorite"

                    onTriggered: {
                        if(viewMode == "favorites") {
                            var vevent = { uid: model.uid } ;
                            _eventController.remove(root.rwCalendar, vevent);
                            eventsUpdated();
                        }
                        else {
                            console.log("Adding event to favorites");
                            var vevent = { "uid" : model.uid, "startDate": model.dtstart, "summary": model.summary, "description": model.description, "allDay": model.allDay, "location": model.location, "endDate": model.dtend, "categories": model.eventCategories, "url": model.url/*"alarms": incidenceAlarmsModel.alarms()*/};

                            var addEditResult = _eventController.addEdit(root.rwCalendar, vevent);

                            if(addEditResult == 201)
                            {
                                showPassiveNotification(i18n("Added to favorites"));
                            }
                            else if (addEditResult == 304)
                            {
                                showPassiveNotification(i18n("Already in favorites"));
                            }
                        }
                    }
                }
            ]

            contentItem: RowLayout {
                spacing: Kirigami.Units.largeSpacing * 2
                width: parent.width

                Controls2.Label {
                    property string timeFormat: model.allday ? "" : "hh:mm"

                    width: Kirigami.Units.gridUnit * 15

                    wrapMode: Text.WordWrap
                    text: ((model.dtstart && !isNaN(model.dtstart)) ? model.dtstart.toLocaleString(Qt.locale(), timeFormat ) : "") +
                        (model.dtend && !isNaN(model.dtend) ? "-" +
                            model.dtend.toLocaleString(Qt.locale(), timeFormat ) : "")
                }

                Controls2.Label {
                    visible: model.summary != ""
                    wrapMode: Text.WordWrap
                    text: model.summary
                    Layout.fillWidth: true
                }
            }
        }
    }

    KDEFOSDEM.EventModel {
        id: eventsModel

        filterdt: root.eventStartDt
        memorycalendar: root.roCalendar.memorycalendar
        eventCategory: root.category
    }

}
