set(HEADERS
    appsettings.h
    dbusproxy.h
    proxytorrentmodel.h
    transmission.h
    torrentmodel.h
    torrentfilemodel.h
    torrentpeermodel.h
    torrenttrackermodel.h
)

set(SOURCES
    tremotesf.cpp
    appsettings.cpp
    dbusproxy.cpp
    proxytorrentmodel.cpp
    transmission.cpp
    torrentmodel.cpp
    torrentfilemodel.cpp
    torrentpeermodel.cpp
    torrenttrackermodel.cpp
    folderlistmodel/qquickfolderlistmodel.cpp
    folderlistmodel/fileinfothread.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR On)
set(CMAKE_AUTOMOC On)

include_directories("${CMAKE_INSTALL_FULL_INCLUDEDIR}/sailfishapp" ${MLITE5_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pie")
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} Qt5::DBus Qt5::Network Qt5::Quick ${SAILFISHAPP_LIBRARIES} ${MLITE5_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_FULL_BINDIR}")
