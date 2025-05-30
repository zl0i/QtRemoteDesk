cmake_minimum_required(VERSION 3.16)

project(FPDeskReceiver VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick WebSockets)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(receiverFPDesk
    main.cpp
)

qt_add_qml_module(receiverFPDesk
    URI FPDeskReceiver
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES videoreceiver.h videoreceiver.cpp
        SOURCES
        SOURCES eventmanager.h eventmanager.cpp
        SOURCES remoteimageprovider.h remoteimageprovider.cpp
        SOURCES eventserializer.h eventserializer.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(receiverFPDesk PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.receiverFPDesk
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(receiverFPDesk
    PRIVATE Qt6::Quick Qt6::WebSockets
)

include(GNUInstallDirs)
install(TARGETS receiverFPDesk
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
