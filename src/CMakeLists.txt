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
target_link_libraries(kongress PRIVATE Qt5::Core Qt5::Qml Qt5::Quick Qt5::Svg Qt5::QuickControls2 Qt5::Network KF5::ConfigCore KF5::I18n KF5::CalendarCore KF5::CoreAddons)
if (BUILD_TESTING)
    target_link_libraries(kongress PRIVATE Qt5::Test)
endif()

target_include_directories(kongress PRIVATE ${CMAKE_BINARY_DIR})

if (ANDROID)
    target_link_libraries(kongress PRIVATE
        KF5::Kirigami2
        Qt5::Svg
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
    )
else()
    target_link_libraries(kongress PRIVATE Qt5::Widgets Qt5::DBus)
endif()

install(TARGETS kongress ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
