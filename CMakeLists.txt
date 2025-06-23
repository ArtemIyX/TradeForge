cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME "TradeForge")
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION
        main.cpp
        Widgets/mainwindow.cpp
        Widgets/mainwindow.h
        Widgets/mainwindow.ui
        Widgets/HistoricalWindow/historicalwindow.cpp
        Widgets/HistoricalWindow/historicalwindow.h
        Widgets/HistoricalWindow/historicalwindow.ui
        Widgets/HistoricalWindow/Components/customStyledItemDelegate.cpp
        Widgets/HistoricalWindow/Components/customStyledItemDelegate.h
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

get_target_property(QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${WINDEPLOYQT_EXECUTABLE}
        --verbose 2
        --no-translations
        --no-system-d3d-compiler
        --no-opengl-sw
        $<TARGET_FILE:${PROJECT_NAME}>
)

set(DLL_FILES
        "D:/Qt/Tools/mingw1310_64/bin/libgcc_s_seh-1.dll"
        "D:/Qt/Tools/mingw1310_64/bin/libstdc++-6.dll"
        "D:/Qt/Tools/mingw1310_64/bin/libwinpthread-1.dll"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL_FILES}
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

qt_generate_deploy_app_script(
    TARGET TradeForge
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

qt_finalize_executable(${PROJECT_NAME})
