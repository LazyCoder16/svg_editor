cmake_minimum_required(VERSION 3.19)
project(svg_editorr LANGUAGES CXX)

find_package(Qt6 6.3 REQUIRED COMPONENTS Core Widgets Gui)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

qt_standard_project_setup()

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE HDR_FILES "include/*.h")
file(GLOB_RECURSE UI_FILES "forms/*.ui")

qt_add_executable(svg_editorr
    WIN32 MACOSX_BUNDLE
    ${SRC_FILES}
    ${HDR_FILES}
    ${UI_FILES}
    resources.qrc
)

target_include_directories(svg_editorr PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(svg_editorr PROPERTIES 
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/forms"
)

target_link_libraries(svg_editorr
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Gui
)

include(GNUInstallDirs)

install(TARGETS svg_editorr
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

#qt_generate_deploy_app_script(
#    TARGET svg_editorr
#    OUTPUT_SCRIPT deploy_script
#    NO_UNSUPPORTED_PLATFORM_ERROR
#)
#install(SCRIPT ${deploy_script})
