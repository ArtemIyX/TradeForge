cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME "TradeForge")
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Charts Concurrent)

qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION
        main.cpp
        resources/resources.qrc
        Widgets/mainwindow.cpp
        Widgets/mainwindow.h
        Widgets/mainwindow.ui
        Widgets/HistoricalWindow/historicalwindow.cpp
        Widgets/HistoricalWindow/historicalwindow.h
        Widgets/HistoricalWindow/historicalwindow.ui
        Widgets/HistoricalWindow/Components/customStyledItemDelegate.cpp
        Widgets/HistoricalWindow/Components/customStyledItemDelegate.h
        Widgets/HistoricalWindow/Components/tableSymbolsStyledDelegate.cpp
        Widgets/HistoricalWindow/Components/tableSymbolsStyledDelegate.h
        Widgets/HistoricalWindow/CustomTable/historicatlWindowTable.cpp
        Widgets/HistoricalWindow/CustomTable/historicatlWindowTable.h
        Widgets/HistoricalWindow/Data/SymbolStructs.cuh
        Widgets/TitleBar/customTitleBar.cpp
        Widgets/TitleBar/customTitleBar.h
        Widgets/CandleCharts/candleChartView.cpp
        Widgets/CandleCharts/candleChartView.h
        Widgets/Subsystems/historicalDataManager.cpp
        Widgets/Subsystems/historicalDataManager.h
        Widgets/HistoricalWindow/ImportFilesDialog/importfileswindow.cpp
        Widgets/HistoricalWindow/ImportFilesDialog/importfileswindow.h
        Widgets/HistoricalWindow/ImportFilesDialog/importfileswindow.ui
        Widgets/HistoricalWindow/ImportFilesDialog/filetoimport.cpp
        Widgets/HistoricalWindow/ImportFilesDialog/filetoimport.h
        Widgets/HistoricalWindow/ImportFilesDialog/filetoimport.ui
        Widgets/HistoricalWindow/DownloadCSVWindow/downloadcsvwindow.cpp
        Widgets/HistoricalWindow/DownloadCSVWindow/downloadcsvwindow.h
        Widgets/HistoricalWindow/DownloadCSVWindow/downloadcsvwindow.ui
        Widgets/HistoricalWindow/CustomMessageBox/custommessagebox.cpp
        Widgets/HistoricalWindow/CustomMessageBox/custommessagebox.h
        Widgets/Subsystems/symboldata.cpp
        Widgets/Subsystems/symboldata.h
        Widgets/Terminal/terminal.cpp
        Widgets/Terminal/terminal.h
        Widgets/Terminal/terminal.ui
        Widgets/Terminal/messageHandler.h
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Charts
        Qt::Concurrent
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/HistoricalWindow/Data
        ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Components
        ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/TitleBar
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
        --compiler-runtime
        $<TARGET_FILE:${PROJECT_NAME}>
)

set(PythonFiles
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils/YahooDownloader.py
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PythonFiles}
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>" "Utils/"
)

qt_generate_deploy_app_script(
    TARGET TradeForge
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

qt_finalize_executable(${PROJECT_NAME})
