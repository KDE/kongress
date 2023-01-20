# SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

project(kongress VERSION)

set(PROJECT_VERSION "22.11")

set(KF_MIN_VERSION "5.95.0")
set(QT_MIN_VERSION "5.15.2")
if (ANDROID)
    set(QT_MIN_VERSION "5.15.8")
endif()
set (KDE_COMPILERSETTINGS_LEVEL "5.82.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(REMINDERS_ENABLED "Build with reminders support" ON)

include(CTest)
include(FeatureSummary)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMPoQmTools)
include(ECMSetupVersion)
include(ECMDeprecationSettings)


ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX KONGRESS
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Quick Gui Svg Qml QuickControls2 Network)

find_package(KF5 ${KF_MIN_VERSION} REQUIRED COMPONENTS Config Kirigami2 I18n CalendarCore CoreAddons)
ecm_set_disabled_deprecation_versions(QT 5.15.2
     KF 5.101.0
    )


if (ANDROID)
    find_package(OpenSSL REQUIRED)
else()
    find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS Widgets)
endif()
if (BUILD_TESTING)
    find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Test)
endif()
if(NOT ANDROID AND REMINDERS_ENABLED)
    find_package(KF5 ${KF_MIN_VERSION} REQUIRED COMPONENTS DBusAddons Notifications)
endif()

add_subdirectory(src)

if(NOT ANDROID AND REMINDERS_ENABLED)
    add_subdirectory(kongressac)
endif()

if (ANDROID)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/android/version.gradle.in ${CMAKE_BINARY_DIR}/version.gradle)
endif()

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
endif()

install(FILES org.kde.kongress.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(PROGRAMS org.kde.kongress.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kongress.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

