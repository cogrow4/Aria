cmake_minimum_required(VERSION 3.16)

project(Aria VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core)

set(PROJECT_SOURCES
    # Main
    src/main.cpp
    
    # Core
    src/core/canvas.cpp
    src/core/canvas.h
    src/core/canvas_methods.cpp
    src/core/brush.cpp
    src/core/brush.h
    src/core/layer.cpp
    src/core/layer.h
    src/core/layermanager.cpp
    src/core/layermanager.h
    
    # UI
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow_fileops.cpp
    
    # UI Panels
    src/ui/panels/brushpanel.cpp
    src/ui/panels/brushpanel.h
    src/ui/panels/layerpanel.cpp
    src/ui/panels/layerpanel.h
    
    # UI Dialogs
    src/ui/dialogs/welcomedialog.cpp
    src/ui/dialogs/welcomedialog.h
    
    # Widgets
    src/widgets/hsvcolorpicker.cpp
    src/widgets/hsvcolorpicker.h
    src/widgets/ariacolorpicker.cpp
    src/widgets/ariacolorpicker.h
    
    # Utils
    src/utils/shortcuts/shortcutmanager.cpp
    src/utils/shortcuts/shortcutmanager.h
)

# Resources
set(PROJECT_RESOURCES
    resources/resources.qrc
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.qrc")
    qt_add_resources(PROJECT_SOURCES resources/resources.qrc)
endif()

qt_add_executable(Aria
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

#  Add include directories for clean includes
target_include_directories(Aria PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/panels
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/shortcuts
)

target_link_libraries(Aria PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core)
