
if (NOT STATIC_LIBRARY)
    ecm_create_qm_loader(kongress_QM_LOADER kongress_qt)
endif()

set(kongress_SRCS
    main.cpp
    settingscontroller.cpp
    calendarcontroller.cpp
    localcalendar.cpp
    eventmodel.cpp
    eventcontroller.cpp
    incidencealarmsmodel.cpp
    conference.cpp
    conferencecontroller.cpp
    conferencemodel.cpp
    )

qt5_add_resources(RESOURCES resources.qrc)

add_executable(kongress ${kongress_SRCS} ${RESOURCES})
target_link_libraries(kongress PRIVATE Qt5::Core Qt5::Qml Qt5::Quick Qt5::Test Qt5::Svg Qt5::Network Qt5::QuickControls2 KF5::ConfigCore KF5::I18n KF5::CalendarCore KF5::CoreAddons KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM)

if (ANDROID)
    target_link_libraries(kongress PRIVATE
        KF5::Kirigami2
        Qt5::Svg
        OpenSSL::SSL
        Qt5::AndroidExtras
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
        view-categories
        settings-configure
    )
else()
    target_link_libraries(kongress PRIVATE Qt5::Widgets)
endif()

install(TARGETS kongress ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES contents/ui/kongress.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)
