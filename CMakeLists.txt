# SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

# KDE Applications version, managed by release script.
set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "11")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kongress VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "6.8.0")
set(KF_MIN_VERSION "6.12.0")

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
include(ECMAddAndroidApk)
include(ECMQmlModule)
include(KDEClangFormat)
include(KDEGitCommitHooks)


ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX KONGRESS
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Quick Gui Svg Qml QuickControls2 Network)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Config Kirigami2 I18n CalendarCore CoreAddons)
find_package(KF6KirigamiAddons 1.0 REQUIRED)
find_package(KOSMIndoorMap 25.08 REQUIRED)
ecm_set_disabled_deprecation_versions(QT 6.9.0 KF 6.13.0)


if (ANDROID)
    find_package(OpenSSL REQUIRED)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS ColorScheme)
else()
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Widgets)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Crash)
endif()
if (BUILD_TESTING)
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Test)
endif()
if(NOT ANDROID AND REMINDERS_ENABLED)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS DBusAddons Notifications)
endif()

add_subdirectory(src)

if(NOT ANDROID AND REMINDERS_ENABLED)
    add_subdirectory(kongressac)
endif()

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
endif()

if (NOT ANDROID)
    install(FILES org.kde.kongress.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
    install(PROGRAMS org.kde.kongress.desktop DESTINATION ${KDE_INSTALL_APPDIR})
    install(FILES org.kde.kongress.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)
else()
    install(FILES org.kde.kongress.svg DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami/breeze-internal/icons/)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
