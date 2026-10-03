cmake_minimum_required(VERSION 3.16)

# Read version from VERSION file
file(READ VERSION VERSION_FILE_CONTENT)
string(STRIP "${VERSION_FILE_CONTENT}" PROJECT_VERSION)

project(deepframe3d VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml QuickControls2 Widgets)

# Sources
set(SOURCES
    src/main.cpp
    src/mpo.cpp
    src/managedimageprovider.cpp
    src/backend.cpp
)

# QML files
set(QML_FILES
    src/main.qml
)

# Resources - use Qt6 resource system
# Must be called before add_executable
qt6_add_resources(QRC_SRCS src/resources.qrc)

# Create executable
add_executable(deepframe3d ${SOURCES} ${QRC_SRCS})

target_link_libraries(deepframe3d PRIVATE Qt6::Core Qt6::Gui Qt6::Quick Qt6::Qml Qt6::QuickControls2 Qt6::Widgets)

# Desktop file installation
install(FILES resources/deepframe3d.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES icons/deepframe3d.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps RENAME deepframe3d.svg)

# MIME type installation
install(FILES mimetypes/image-mpo.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/mime/packages RENAME deepframe3d.xml)

install(TARGETS deepframe3d DESTINATION ${CMAKE_INSTALL_BINDIR})

# CPack configuration for Debian package
include(InstallRequiredSystemLibraries)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "deepframe3d")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Deepframe3D")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A KDE application for viewing MPO (Multi-Picture Object) stereo 3D images")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/user/deepframe3d")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Deepframe3D Team <deepframe3d@example.com>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.27), libqt6core6 (>= 6.0), libqt6gui6 (>= 6.0), libqt6quick6 (>= 6.0), libqt6qml6 (>= 6.0), libqt6quickcontrols2-6 (>= 6.0), libqt6widgets6 (>= 6.0)")
set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

include(CPack)
