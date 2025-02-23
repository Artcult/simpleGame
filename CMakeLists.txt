cmake_minimum_required(VERSION 3.16)

project(Quick-Rock-Paper-Scissors LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

include_directories(include)

add_executable(Quick-Rock-Paper-Scissors
  main.cpp

  include/GameController.h
  include/LobbyClient.h
  include/ServerLobby.h

  include/INetworkSerializable.h
  include/IMainMenu.h
  include/IGameActionMenu.h

  include/LanTcpServer.h
  include/LanTcpClient.h
  include/UdpBroadcaster.h
  include/UdpBroadcastListener.h

  include/PlayerProfile.h
  include/PlayerConnection.h
  include/LobbyInfo.h
  include/GameAction.h
  include/ChatMessage.h

  src/LobbyClient.cpp
  src/GameController.cpp
  src/ServerLobby.cpp

  src/LanTcpServer.cpp
  src/LanTcpClient.cpp
  src/UdpBroadcaster.cpp
  src/UdpBroadcastListener.cpp


  include/ConsoleGameAction.h
  src/ConsoleGameAction.cpp
  src/ConsoleMainMenu.cpp
  include/ConsoleMainMenu.h

)
target_link_libraries(Quick-Rock-Paper-Scissors Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(Quick-Rock-Paper-Scissors Qt${QT_VERSION_MAJOR}::Network)

include(GNUInstallDirs)
install(TARGETS Quick-Rock-Paper-Scissors
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(WIN32)
    add_custom_command(TARGET Quick-Rock-Paper-Scissors POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt dependencies..."
        COMMAND ${WINDEPLOYQT_EXECUTABLE} --release $<TARGET_FILE:Quick-Rock-Paper-Scissors>
        COMMENT "Running windeployqt to include dependencies"
        VERBATIM
    )
endif()
