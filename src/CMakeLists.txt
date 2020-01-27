
if (NOT STATIC_LIBRARY)
    ecm_create_qm_loader(kongress_QM_LOADER kongress_qt)
endif()

set(kongress_SRCS
    main.cpp
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
target_link_libraries(kongress Qt5::Core Qt5::Qml Qt5::Quick Qt5::Test Qt5::Svg Qt5::Network KF5::ConfigCore KF5::I18n KF5::CalendarCore)

if(ANDROID)
    kirigami_package_breeze_icons(ICONS
        delete
        documentinfo
        edit-select
        exchange-positions
        favorite
        find-location
        group
        internet-services
        view-calendar-agenda
        view-calendar-day
        view-categories
    )
endif()

install(TARGETS kongress ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES contents/ui/kongress.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)
