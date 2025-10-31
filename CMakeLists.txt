cmake_minimum_required(VERSION 3.19)
project(TimelineFinder LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(TimelineFinder
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    filemodel.h filemodel.cpp
    pointsmodel.h pointsmodel.cpp
    textmodel.h textmodel.cpp
    treeitem.h treeitem.cpp
    parseandread.h parseandread.cpp
    readfiles.h readfiles.cpp
    treeitemcache.h treeitemcache.cpp
)

target_link_libraries(TimelineFinder
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS TimelineFinder
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET TimelineFinder
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
