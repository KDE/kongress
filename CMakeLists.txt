cmake_minimum_required(VERSION 3.0)

project(kongress)

set(KF5_MIN_VERSION "5.63.0")
set(QT_MIN_VERSION "5.7.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(REMINDERS_ENABLED "Build with reminders support" ON)

include(FeatureSummary)

################# set KDE specific information #################

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMPoQmTools)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS DBus Core Quick Gui Svg Test Qml QuickControls2 Network)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Config Kirigami2 I18n CalendarCore CoreAddons KIO)

if (ANDROID)
    # runtime dependencies are build-time dependencies on Android
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS AndroidExtras)
    find_package(OpenSSL REQUIRED)
else()
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Widgets)
endif()

if(NOT ANDROID AND REMINDERS_ENABLED)
    find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS DBusAddons Notifications Service)
endif()

################# now find all used packages #################

find_package(PkgConfig)
#########################################################################

add_subdirectory(src)

if(REMINDERS_ENABLED)
    add_subdirectory(kongressac)
endif()

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()

install(FILES org.kde.kongress.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(PROGRAMS org.kde.kongress.desktop DESTINATION ${KDE_INSTALL_APPDIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

