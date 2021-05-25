# SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.0)

project(kongress VERSION)

set(PROJECT_VERSION "21.05")

set(KF5_MIN_VERSION "5.63.0")
set(QT_MIN_VERSION "5.7.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(REMINDERS_ENABLED "Build with reminders support" ON)

include(CTest)
include(FeatureSummary)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMPoQmTools)
include(ECMSetupVersion)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX KONGRESS
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Quick Gui Svg Qml QuickControls2 Network)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Config Kirigami2 I18n CalendarCore CoreAddons)

if (ANDROID)
    # runtime dependencies are build-time dependencies on Android
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS AndroidExtras)
    find_package(OpenSSL REQUIRED)
else()
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Widgets)
endif()
if (BUILD_TESTING)
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Test)
endif()
if(NOT ANDROID AND REMINDERS_ENABLED)
    find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS DBusAddons Notifications)
endif()

add_subdirectory(src)

if(NOT ANDROID AND REMINDERS_ENABLED)
    add_subdirectory(kongressac)
endif()

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
endif()

install(FILES org.kde.kongress.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(PROGRAMS org.kde.kongress.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kongress.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

