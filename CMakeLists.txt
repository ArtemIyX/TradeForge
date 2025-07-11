cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME "TradeForge")
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Charts Concurrent)

set(VERSION_FILE "${CMAKE_SOURCE_DIR}/version.txt")

function(increment_version)
    if(EXISTS ${VERSION_FILE})
        file(READ ${VERSION_FILE} CURRENT_VERSION)

        string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+)" _ ${CURRENT_VERSION})
        set(MAJOR ${CMAKE_MATCH_1})
        set(MINOR ${CMAKE_MATCH_2})
        set(PATCH ${CMAKE_MATCH_3})

        math(EXPR NEW_PATCH "${PATCH} + 1")

        set(NEW_VERSION "${MAJOR}.${MINOR}.${NEW_PATCH}")
    else()
        set(NEW_VERSION "1.0.0")
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        file(WRITE ${VERSION_FILE} ${NEW_VERSION})
        message(STATUS "Version incremented to: ${NEW_VERSION}")
    endif()

    set(PROJECT_VERSION ${NEW_VERSION} PARENT_SCOPE)
endfunction()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    increment_version()
else()
    if(EXISTS ${VERSION_FILE})
        file(READ ${VERSION_FILE} PROJECT_VERSION)
    else()
        set(PROJECT_VERSION "1.0.0")
    endif()
endif()


qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION WIN32
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
        Widgets/CustomMessageBoxes/messageBoxFactory.cpp
        Widgets/CustomMessageBoxes/messageBoxFactory.h
        Widgets/HistoricalWindow/Data/e_HistoricalWindowItemData.h
        Widgets/CustomMessageBoxes/Windows/InfoMessageBox/custominfomessagebox.cpp
        Widgets/CustomMessageBoxes/Windows/InfoMessageBox/custominfomessagebox.h
        Widgets/CustomMessageBoxes/Windows/InfoMessageBox/custominfomessagebox.ui
        Widgets/CustomMessageBoxes/Windows/AcceptMessageBox/customacceptmessagebox.cpp
        Widgets/CustomMessageBoxes/Windows/AcceptMessageBox/customacceptmessagebox.h
        Widgets/CustomMessageBoxes/Windows/AcceptMessageBox/customacceptmessagebox.ui
        Widgets/CustomMessageBoxes/Windows/ProgressMessageBox/customprogressmessagebox.cpp
        Widgets/CustomMessageBoxes/Windows/ProgressMessageBox/customprogressmessagebox.h
        Widgets/CustomMessageBoxes/Windows/ProgressMessageBox/customprogressmessagebox.ui
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
        --verbose 1
        --no-translations
        --no-system-d3d-compiler
        --no-opengl-sw
        --compiler-runtime
        $<TARGET_FILE:${PROJECT_NAME}>
)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Utils")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Utils/YahooDownloader.py" "${CMAKE_BINARY_DIR}/Utils/YahooDownloader.py" COPYONLY)

set(RELEASE_DIR "${CMAKE_BINARY_DIR}/TradeForge ${PROJECT_VERSION}")
set(RELEASE_BIN_DIR "${RELEASE_DIR}/bin")
set(RELEASE_RESOURCES_DIR "${RELEASE_DIR}/resources")
set(RELEASE_UTILS_DIR "${RELEASE_DIR}/utils")

file(MAKE_DIRECTORY "${RELEASE_DIR}")
file(MAKE_DIRECTORY "${RELEASE_BIN_DIR}")
file(MAKE_DIRECTORY "${RELEASE_RESOURCES_DIR}")
file(MAKE_DIRECTORY "${RELEASE_UTILS_DIR}")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:${PROJECT_NAME}>"
        "${RELEASE_BIN_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        COMMENT "Copying executable to release directory"
)

file(GLOB RESOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")
foreach(RESOURCE_FILE ${RESOURCE_FILES})
    get_filename_component(FILE_NAME ${RESOURCE_FILE} NAME)
    configure_file("${RESOURCE_FILE}" "${RELEASE_RESOURCES_DIR}/${FILE_NAME}" COPYONLY)
endforeach()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Utils/YahooDownloader.py" "${RELEASE_UTILS_DIR}/YahooDownloader.py" COPYONLY)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${WINDEPLOYQT_EXECUTABLE}
        --verbose 1
        --no-translations
        --no-system-d3d-compiler
        --no-opengl-sw
        --compiler-runtime
        --dir "${RELEASE_BIN_DIR}"
        "$<TARGET_FILE:${PROJECT_NAME}>"
        COMMENT "Deploying Qt DLLs and plugins to ${RELEASE_BIN_DIR}"
        DEPENDS ${PROJECT_NAME}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

qt_finalize_executable(${PROJECT_NAME})
