project(harbour-tremotesf)
cmake_minimum_required(VERSION 2.8.11)

include(GNUInstallDirs)

find_package(Qt5DBus REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Quick REQUIRED)

find_package(PkgConfig)
pkg_check_modules(SAILFISHAPP sailfishapp)
pkg_check_modules(MLITE5 mlite5)

install(DIRECTORY qml DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}")
install(FILES cover.png DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}")
install(FILES ${PROJECT_NAME}.desktop DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/applications")
install(FILES ${PROJECT_NAME}.png DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/icons/hicolor/86x86/apps")

add_subdirectory(src)
add_subdirectory(translations)
