add_definitions(-DTRANSLATION_DOMAIN=\"kongressac\")

set(kongressac_SRCS
    calalarmclient.cpp
    kongressacmain.cpp
    alarmsmodel.cpp
    alarmnotification.cpp
    notificationhandler.cpp
    )

qt5_add_dbus_adaptor(kongressac_SRCS org.kde.kongressac.xml calalarmclient.h CalAlarmClient)

add_executable(kongressac ${kongressac_SRCS} ${RESOURCES})

target_link_libraries(kongressac
    KF5::CalendarCore
    KF5::DBusAddons
    KF5::Notifications
    KF5::Service
    KF5::I18n
    Qt5::DBus
    )

install(TARGETS
    kongressac ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    )

install(FILES
    org.kde.kongressac.desktop
    DESTINATION ${KDE_INSTALL_AUTOSTARTDIR}
    )

install(FILES
    org.kde.kongressac.xml
    DESTINATION ${DBUS_INTERFACES_INSTALL_DIR}
    )

install(FILES kongressac.notifyrc DESTINATION ${KNOTIFYRC_INSTALL_DIR})

