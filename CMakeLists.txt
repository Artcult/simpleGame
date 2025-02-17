cmake_minimum_required(VERSION 3.16)

project(simpleGame LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

add_executable(simpleGame
  main.cpp

  PlayerProfile.h
  INetworkSerializable.h
  IMainMenu.h
  ConsoleMainMenu.h ConsoleMainMenu.cpp
  LobbyClient.h LobbyClient.cpp
  MainMenuController.h MainMenuController.cpp
  ServerLobby.h ServerLobby.cpp
)
target_link_libraries(simpleGame Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(simpleGame Qt${QT_VERSION_MAJOR}::Network)

include(GNUInstallDirs)
install(TARGETS simpleGame
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(WIN32)
    add_custom_command(TARGET simpleGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt dependencies..."
        COMMAND ${WINDEPLOYQT_EXECUTABLE} --release $<TARGET_FILE:simpleGame>
        COMMENT "Running windeployqt to include dependencies"
        VERBATIM
    )
endif()
