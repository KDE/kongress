# SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
#
# SPDX-License-Identifier: BSD-2-Clause

add_definitions(-DTRANSLATION_DOMAIN=\"kongressac\")

set(kongressac_SRCS
    calalarmclient.cpp
    kongressacmain.cpp
    alarmsmodel.cpp
    alarmnotification.cpp
    notificationhandler.cpp
    solidwakeupbackend.cpp
    wakeupmanager.cpp
)

qt5_add_dbus_adaptor(kongressac_SRCS org.kde.kongressac.xml calalarmclient.h CalAlarmClient)
qt5_add_dbus_adaptor(kongressac_SRCS org.kde.wakeupmanager.PowerManagement.xml wakeupmanager.h WakeupManager)

add_executable(kongressac ${kongressac_SRCS} ${RESOURCES})

target_link_libraries(kongressac
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::CalendarCore
    KF5::DBusAddons
    KF5::Notifications
    KF5::I18n
    Qt5::DBus
    )

install(TARGETS
    kongressac ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    )

install(FILES kongressac.notifyrc DESTINATION ${KNOTIFYRC_INSTALL_DIR})
set(SERV_EXEC ${KDE_INSTALL_FULL_BINDIR}/kongressac)
configure_file(org.kde.kongressac.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kongressac.service)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kongressac.service DESTINATION ${KDE_INSTALL_FULL_DBUSSERVICEDIR})
