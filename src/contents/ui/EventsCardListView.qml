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
import org.kde.kirigami 2.4 as Kirigami
import org.kde.kongress 0.1 as Kongress

Kirigami.Page {
    id: root

    property string viewMode
    property date eventStartDt
    property var roCalendar
    property var rwCalendar
    property string category
    property bool showCategories: true

    signal eventsUpdated

    function reload()
    {
        cardsListview.model.loadEvents();
    }

    leftPadding: 0
    rightPadding: 0

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
            viewMode: root.viewMode

            onEventUpdated: root.eventsUpdated()
        }
    }

    Kirigami.CardsListView {
        id: cardsListview

        anchors.fill: parent

        model: eventsModel

        delegate: Kirigami.Card {
            id: cardDelegate

            banner.title: model.summary
            banner.titleLevel: 3

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

            contentItem: Column {
                spacing: Kirigami.Units.largeSpacing
                topPadding: 0
                bottomPadding: 0
                RowLayout {
                    width: cardDelegate.availableWidth
                    spacing: Kirigami.Units.smallSpacing

                    Kirigami.Icon {
                        source: "view-calendar-day"
                        width: Kirigami.Units.iconSizes.small
                        height: width
                    }

                    Controls2.Label {
                        property bool sameEndStart : model.dtstart && !isNaN(model.dtstart) && model.dtend && !isNaN(model.dtend) && model.dtstart.toLocaleString(Qt.locale(), "dd.MM.yyyy") == model.dtend.toLocaleString(Qt.locale(), "dd.MM.yyyy")
                        property string timeFormat: model.allday ? "" : "hh:mm"
                        property string dateFormat: model.allday ? "ddd d MMM yyyy" : "ddd d MMM yyyy hh:mm"
                        property string separator: model.allday ? "" : " - "

                        wrapMode: Text.WordWrap
                        text: ((model.dtstart && !isNaN(model.dtstart)) ? model.dtstart.toLocaleString(Qt.locale(), dateFormat ) : "") +
                            (model.dtend && !isNaN(model.dtend) ? separator +
                                model.dtend.toLocaleString(Qt.locale(), sameEndStart ? timeFormat : dateFormat ) : "")
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    visible: model.location != ""
                    width: cardDelegate.availableWidth
                    spacing: Kirigami.Units.smallSpacing

                    Kirigami.Icon {
                        source: "find-location"
                        width: Kirigami.Units.iconSizes.small
                        height: width
                    }

                    Controls2.Label {
                        wrapMode: Text.WordWrap
                        text: model.location
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    visible: root.showCategories && (model.eventCategories != "")
                    width: cardDelegate.availableWidth
                    spacing: Kirigami.Units.smallSpacing

                    Kirigami.Icon {
                        source: "view-categories"
                        width: Kirigami.Units.iconSizes.small
                        height: width
                    }

                    Controls2.Label {
                        wrapMode: Text.WordWrap
                        text: model.eventCategories
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }

    Kongress.EventModel {
        id: eventsModel

        filterdt: root.eventStartDt
        memorycalendar: root.roCalendar.memorycalendar
        eventCategory: root.category
    }

}
