cmake_minimum_required(VERSION 3.14)

project(QTileLayout LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(
    include
)

add_library(QTileLayout SHARED
  include/QTileLayout/QTileLayout_global.h
  include/QTileLayout/qtilelayout.h
  include/QTileLayout/qtilewidget.h
  src/qtilelayout.cpp
  src/qtilewidget.cpp
)

qt_add_executable(QTileLayoutTest WIN32 MACOSX_BUNDLE
    test/main.cpp
    test/testwindow.cpp
    test/testwindow.h
)

target_link_libraries(QTileLayout PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(QTileLayoutTest PRIVATE Qt${QT_VERSION_MAJOR}::Widgets QTileLayout)

target_compile_definitions(QTileLayout PRIVATE QTILELAYOUT_LIBRARY)
target_compile_definitions(QTileLayoutTest PRIVATE QTILELAYOUTTEST_LIBRARY)

include(GNUInstallDirs)
install(TARGETS QTileLayout
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

qt_generate_deploy_app_script(
    TARGET QTileLayout
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
