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
    calalarmclient.h
    alarmsmodel.h
    alarmnotification.h
    notificationhandler.h
    solidwakeupbackend.h
    wakeupmanager.h
)

qt_add_dbus_adaptor(kongressac_SRCS org.kde.kongressac.xml calalarmclient.h CalAlarmClient)
qt_add_dbus_adaptor(kongressac_SRCS org.kde.wakeupmanager.PowerManagement.xml wakeupmanager.h WakeupManager)

add_executable(kongressac ${kongressac_SRCS} ${RESOURCES})

target_link_libraries(kongressac
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::CalendarCore
    KF6::DBusAddons
    KF6::Notifications
    KF6::I18n
    KF6::Crash
    Qt::DBus
    )

install(TARGETS
    kongressac ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    )

install(FILES kongressac.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
set(SERV_EXEC ${KDE_INSTALL_FULL_BINDIR}/kongressac)
configure_file(org.kde.kongressac.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kongressac.service)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kongressac.service DESTINATION ${KDE_INSTALL_FULL_DBUSSERVICEDIR})
