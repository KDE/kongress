add_definitions(-DTRANSLATION_DOMAIN=\"kdefosdemac\")

set(kdefosdemac_SRCS
    calalarmclient.cpp
    kdefosdemacmain.cpp
    alarmsmodel.cpp
    alarmnotification.cpp
    notificationhandler.cpp
    )

qt5_add_dbus_adaptor(kdefosdemac_SRCS org.kde.kdefosdemac.xml calalarmclient.h CalAlarmClient)

add_executable(kdefosdemac ${kdefosdemac_SRCS} ${RESOURCES})

target_link_libraries(kdefosdemac
    KF5::CalendarCore
    KF5::DBusAddons
    KF5::Notifications
    KF5::Service
    KF5::I18n
    Qt5::DBus
    )

install(TARGETS
    kdefosdemac ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    )

install(FILES
    org.kde.kdefosdemac.desktop
    DESTINATION ${KDE_INSTALL_AUTOSTARTDIR}
    )

install(FILES
    org.kde.kdefosdemac.xml
    DESTINATION ${DBUS_INTERFACES_INSTALL_DIR}
    )

install(FILES kdefosdemac.notifyrc DESTINATION ${KNOTIFYRC_INSTALL_DIR})

