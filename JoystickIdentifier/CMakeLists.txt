cmake_minimum_required(VERSION 3.16)

project(JoystickIdentifier VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Qt6 prefix path (adjust if needed)
set(CMAKE_PREFIX_PATH "/home/med/Qt/6.8.1/gcc_64/lib/cmake")

# Find Qt6 required components.
find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets Core)
find_package(X11 REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -D_DEBUG")

# Add QJoysticks as a subdirectory.
add_subdirectory(../QJoysticks ${CMAKE_BINARY_DIR}/QJoysticks)

# Set up project sources.
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    signalhandler.cpp
    signalhandler.h
)

# Create executable using Qt6.
qt_add_executable(JoystickIdentifier
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    mainwindow.ui
)

# Add subdirectory for VideoReceiver.
add_subdirectory(VideoReceiver)


add_library(siyi-sdk STATIC
    thirdparty/SIYI-SDK/src/sdk.cpp
    thirdparty/SIYI-SDK/src/message.cpp
    thirdparty/SIYI-SDK/src/crc16.cpp
)
target_include_directories(siyi-sdk PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SIYI-SDK/src
)

# Link libraries.
target_link_libraries(JoystickIdentifier PRIVATE
    ${X11_LIBRARIES}
    VideoReceiver
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    QJoysticks
    #${CMAKE_SOURCE_DIR}/thirdparty/SIYI-SDK/build/libsiyi-sdk-static.a
    siyi-sdk
)

# Additional video recorder sources.
set(VIDEO_RECORDER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/VideoRecorderApp/VideoRecorderWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/VideoReceiver/VideoReceiver.cpp
)

set(VIDEO_RECORDER_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/VideoRecorderApp/VideoRecorderWidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/VideoReceiver/VideoReceiver.h
)



target_sources(JoystickIdentifier PRIVATE ${VIDEO_RECORDER_SOURCES} ${VIDEO_RECORDER_HEADERS})
target_include_directories(JoystickIdentifier PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SIYI-SDK/src
    ${CMAKE_CURRENT_SOURCE_DIR}/VideoRecorderApp
    ${CMAKE_CURRENT_SOURCE_DIR}/VideoReceiver
)

include(GNUInstallDirs)
install(TARGETS JoystickIdentifier
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(JoystickIdentifier)

