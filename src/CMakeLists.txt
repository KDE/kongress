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
    resources.qrc
    )

add_executable(kongress ${kongress_SRCS} ${RESOURCES})
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
        delete
        documentinfo
        edit-select
        dialog-warning-symbolic
        exchange-positions
        favorite
        find-location
        group
        internet-services
        view-calendar-agenda
        view-calendar-day
        settings-configure
        category
        help-about-symbolic
        document-export
        folder-open
        draw-arrow-back
        update-none
        emblem-warning
        emblem-error
        arrow-down
        arrow-up
    )

    ecm_add_android_apk(kongress ANDROID_DIR ${CMAKE_SOURCE_DIR}/android)
else()
    target_link_libraries(kongress PRIVATE Qt::Widgets Qt::DBus)
endif()

install(TARGETS kongress ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
