cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(PLUGIN "Example")

set(
    SRC
    plugin.cpp
    reader.cpp
    service.cpp
    servicehandler.cpp
)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Qml Quick DBus)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Service instatiation
set(daemon_xml ${CMAKE_SOURCE_DIR}/plugins/Example/org.sailfishos.nfc.Daemon.xml)
qt5_add_dbus_interface(SRC ${daemon_xml} daemon_interface)

# NFC Reader
set(adapter_xml ${CMAKE_SOURCE_DIR}/plugins/Example/org.sailfishos.nfc.Adapter.xml)
qt5_add_dbus_interface(SRC ${adapter_xml} adapter_interface)
set(peer_xml ${CMAKE_SOURCE_DIR}/plugins/Example/org.sailfishos.nfc.Peer.xml)
qt5_add_dbus_interface(SRC ${peer_xml} peer_interface)

add_library(${PLUGIN} MODULE ${SRC})
target_link_libraries(${PLUGIN} Qt5::Qml Qt5::Quick Qt5::DBus)
set_target_properties(${PLUGIN} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PLUGIN})

execute_process(
    COMMAND dpkg-architecture -qDEB_HOST_MULTIARCH
    OUTPUT_VARIABLE ARCH_TRIPLET
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(QT_IMPORTS_DIR "/lib/${ARCH_TRIPLET}")

install(TARGETS ${PLUGIN} DESTINATION ${QT_IMPORTS_DIR}/${PLUGIN}/)
install(FILES qmldir DESTINATION ${QT_IMPORTS_DIR}/${PLUGIN}/)
