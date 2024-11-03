cmake_minimum_required(VERSION 3.16)

# 设置项目名称和版本
project(AutoGo VERSION 2.1.1 LANGUAGES CXX)

# 指定 C++ 版本
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(DISABLE_RANDOM_CLICK)

# 查找 Qt 包
find_package(Qt6 REQUIRED COMPONENTS Core Quick Gui Qml QuickControls2)

qt_standard_project_setup()

# 设置源文件和头文件
# 添加头文件和源文件
set(SOURCES
    src/AutoGoInteractor.cpp
    src/BoardData.cpp
    src/GameAnalyzer.cpp
    src/GameBoardHandler.cpp
    src/GameData.cpp
    src/GameInteractor.cpp
    src/Global.cpp
    src/GoBoardItem.cpp
    src/InteractiveInterface/AdbHandle.cpp
    src/InteractiveInterface/AdbInteractor.cpp
    src/InteractiveInterface/AdbScreencaptor.cpp
    src/InteractiveInterface/Controller.cpp
    src/InteractiveInterface/MaaController.cpp
    src/InteractiveInterface/MumuHandle.cpp
    src/InteractiveInterface/MumuScreencaptor.cpp
    src/InteractiveInterface/Screencaptor.cpp
    src/KatagoAnalysisInteractor.cpp
    src/KatagoGTPInteractor.cpp
    src/KatagoInteractor.cpp
    src/Settings.cpp
    src/StoneData.cpp
    src/Util.cpp
    src/main.cpp
)

set(HEADERS
    src/AutoGoInteractor.h
    src/BoardData.h
    src/GameAnalyzer.h
    src/GameBoardHandler.h
    src/GameData.h
    src/GameInteractor.h
    src/Global.h
    src/GoBoardItem.h
    src/InteractiveInterface/AdbHandle.h
    src/InteractiveInterface/AdbInteractor.h
    src/InteractiveInterface/AdbScreencaptor.h
    src/InteractiveInterface/Controller.h
    src/InteractiveInterface/MaaController.h
    src/InteractiveInterface/MumuHandle.h
    src/InteractiveInterface/MumuScreencaptor.h
    src/InteractiveInterface/Screencaptor.h
    src/KatagoAnalysisInteractor.h
    src/KatagoGTPInteractor.h
    src/KatagoInteractor.h
    src/Settings.h
    src/StoneData.h
    src/Util.h
)

# 添加资源文件
set(RESOURCES resources/resources.qrc)

qt_add_resources(RESOURCES ${RESOURCES})

# 创建可执行文件
qt_add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

include_directories(src)
include_directories(${CMAKE_SOURCE_DIR})

qt_add_qml_module(${PROJECT_NAME}
    URI com.LFWQSP2641.qmlcomponents
    VERSION 1.0
    SOURCES
        src/GameBoardHandler.h
        src/GameBoardHandler.cpp
        src/GoBoardItem.h
        src/GoBoardItem.cpp
)

# 添加 OpenCV 库
if(WIN32)
    find_package(OpenCV REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

# 包含 Qt6 的头文件和库
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Quick Qt6::Gui Qt6::Qml Qt6::QuickControls2)

# 安装目标
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

if(NOT ${DEVELOPMENT_MODE})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
    )
    add_compile_definitions(QT_NO_DEBUG_OUTPUT)
endif()
