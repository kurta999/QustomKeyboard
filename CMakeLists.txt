cmake_minimum_required(VERSION 3.5)


project(Graphs VERSION 0.1 LANGUAGES C CXX)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

set_property(SOURCE CmdExecutorTab.cpp PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE mainwindow.cpp PROPERTY SKIP_AUTOMOC ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Boost 1.81.0 COMPONENTS filesystem regex program_options) 

set(USE_HIDAPI true CACHE BOOL "Use HIDAPI library?")
if(USE_HIDAPI)
	#find_package(hidapi REQUIRED)
	add_definitions(-DUSE_HIDAPI)
endif()

set(PROJECT_SOURCES
        main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/mainwindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/mainwindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/mainwindow.ui        
		${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CanTab/BitEditorDialog.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CanTab/BitEditorDialog.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CanTab/CanScriptTab.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CanTab/CanScriptTab.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CanTab/CanLogTab.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CanTab/CanLogTab.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CanTab/CanTab.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CanTab/CanTab.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CanTab/CanTab.ui
		${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CanTab/CanUdsRawDialog.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CanTab/CanUdsRawDialog.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/StructParserTab.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/StructParserTab.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/MapConverterTab.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/MapConverterTab.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/StringEscaperTab.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/StringEscaperTab.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CmdExecutorTab.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/CmdExecutorTab.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/LoggerTab.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/LoggerTab.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/BalloonTip.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/BalloonTip.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/gui/DidTab.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/DidTab.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/ApplicationBase.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/AntiLock.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CanEntryHandler.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CanScriptHandler.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CanDeviceLawicel.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CanDeviceStm32.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CanSerialPort.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CryptoPrice.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CmdExecutor.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CustomMacro.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CorsairHid.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/DatabaseImpl.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/DatabaseLogic.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/DirectoryBackup.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/DidHandler.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/ExceptionHandler.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/IdlePowerSaver.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Logger.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/MacroRecorder.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/PathSeparator.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/PrintScreenSaver.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Sensors.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Server.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Session.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/pch.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Settings.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/StructParser.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/SymlinkCreator.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/TerminalHotkey.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/TcpMessageExecutor.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/MapConverter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/SerialPort.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/SerialForwarder.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/StringToCEscaper.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Utils.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/utils/AsyncSerial.cpp
)

add_subdirectory(libs)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(libs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/interface)  

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DWIN32 -D_WINDOWS -D_WIN32_WINNT=0x0601)
	add_definitions("/wd4267 /wd4244 /wd4828 /wd4267 /wd4244 /MP")
	add_definitions("-DNOMINMAX")

else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-return-type")
endif()

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/Graphs.rc")
qt_add_executable(${PROJECT_NAME} application.qrc MANUAL_FINALIZATION ${PROJECT_SOURCES} ${CMAKE_DL_LIBS} ${app_icon_resource_windows})

target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.hpp)

# Resources:
set(systray_resource_files
    "images/Graphs.ico"
    "images/icon.png"
)

qt_add_resources(${PROJECT_NAME} "systray"
    PREFIX
        "/"
    FILES
        ${systray_resource_files}
)

message("test " libs)

if(USE_HIDAPI)
target_link_libraries(${PROJECT_NAME} PRIVATE $<TARGET_OBJECTS:libs> ${Boost_LIBRARIES} Qt${QT_VERSION_MAJOR}::Widgets ${CMAKE_CURRENT_SOURCE_DIR}/hidapi.lib PowrProf.lib)
else()
target_link_libraries(${PROJECT_NAME} PRIVATE $<TARGET_OBJECTS:libs> ${Boost_LIBRARIES} Qt${QT_VERSION_MAJOR}::Widgets PowrProf.lib)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION . LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

qt_finalize_executable(Graphs)

