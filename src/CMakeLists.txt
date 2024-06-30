# SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
#
# SPDX-License-Identifier: BSD-2-Clause

set(kongress_SRCS
    main.cpp
    settingscontroller.cpp
    calendarcontroller.cpp
    localcalendar.cpp
    eventmodel.cpp
    eventcontroller.cpp
    conference.cpp
    conferencecontroller.cpp
    conferencemodel.cpp
    alarmchecker.cpp
    )

add_executable(kongress ${kongress_SRCS} ${RESOURCES})

ecm_add_qml_module(kongress URI "org.kde.kongress" VERSION 0.1)

ecm_target_qml_sources(kongress SOURCES
    contents/ui/Main.qml
    contents/ui/GlobalDrawer.qml
    contents/ui/ScheduleView.qml
    contents/ui/FavoritesView.qml
    contents/ui/IndoorMapView.qml
    contents/ui/MapView.qml
    contents/ui/Conferences.qml
    contents/ui/EventInfo.qml
    contents/ui/SettingsView.qml
)

target_link_libraries(kongress PRIVATE Qt::Core Qt::Qml Qt::Quick Qt::Svg Qt::QuickControls2 Qt::Network KF6::ConfigCore KF6::I18n KF6::CalendarCore KF6::CoreAddons)
if (BUILD_TESTING)
    target_link_libraries(kongress PRIVATE Qt::Test)
endif()

target_include_directories(kongress PRIVATE ${CMAKE_BINARY_DIR})

if (ANDROID)
    target_link_libraries(kongress PRIVATE
        KF6::Kirigami2
        Qt::Svg
        OpenSSL::SSL
    )

    kirigami_package_breeze_icons(ICONS
        arrow-down
        arrow-up
        category
        delete
        dialog-warning-symbolic
        documentinfo
        document-export
        draw-arrow-back
        edit-select
        emblem-error
        emblem-warning
        exchange-positions
        favorite
        find-location
        folder-open
        go-next
        group
        help-about-symbolic
        internet-services
        map-globe
        map-symbolic
        search
        settings-configure
        update-none
        view-calendar-agenda
        view-calendar-day
    )

    ecm_add_android_apk(kongress ANDROID_DIR ${CMAKE_SOURCE_DIR}/android)
else()
    target_link_libraries(kongress PRIVATE Qt::Widgets Qt::DBus)
endif()

install(TARGETS kongress ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
