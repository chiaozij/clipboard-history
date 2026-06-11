# ============================================================
# CMakeLists.txt — 项目 CMake 构建配置文件
# ============================================================
# 使用 CMake + Qt6 + C++17 构建剪贴板历史记录应用。
#
# 在 MSYS2 UCRT64 环境下构建步骤：
#   1. 打开 D:\MSYS2\ucrt64.exe
#   2. cd /d/Vibe_code
#   3. mkdir build && cd build
#   4. cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..
#   5. cmake --build .
#
# 编译输出：
#   build/ClipboardHistory.exe  — 可直接双击运行
#
# 静态链接（可选，去掉 Qt DLL 依赖）：
#   cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF ..
#   注意：静态构建需要安装 Qt 静态库（MSYS2 pacman -S mingw-w64-ucrt-x86_64-qt6-static）

cmake_minimum_required(VERSION 3.16)

# ==================== 项目信息 ====================
project(ClipboardHistory
    VERSION 1.0.0
    DESCRIPTION "Windows 剪贴板历史记录小应用"
    LANGUAGES CXX
)

# ==================== C++ 标准 ====================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==================== Qt 配置 ====================
# 自动查找 Qt6；若未安装则报错
find_package(Qt6 REQUIRED COMPONENTS
    Core       # QTimer、信号槽等核心功能
    Gui        # QPixmap、QImage 等图形支持
    Widgets    # QWidget、QLineEdit、QScrollArea 等控件
)

# 启用 Qt 的自动 MOC/UIC/RCC 处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ==================== 源文件 ====================
set(SOURCES
    src/main.cpp
    src/ClipboardItem.cpp
    src/ClipboardManager.cpp
    src/HistoryItemWidget.cpp
    src/HistoryPanel.cpp
    src/ThemeManager.cpp
    src/TrayManager.cpp
)

set(HEADERS
    src/ClipboardItem.h
    src/ClipboardManager.h
    src/HistoryItemWidget.h
    src/HistoryPanel.h
    src/TrayManager.h
)

set(RESOURCES
    src/resources/app.qrc
)

# ==================== 可执行文件 ====================
# WIN32 标记表示 Windows GUI 应用（不显示控制台窗口）
add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# ==================== 链接 Qt 库 ====================
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    dwmapi       # Windows DWM API (毛玻璃效果)
    user32       # Windows User API
)

# ==================== 编译器选项 ====================
# MSVC / MinGW 通用选项
target_compile_options(${PROJECT_NAME} PRIVATE
    # 启用常用警告
    -Wall
    -Wextra
)

# MinGW 特定：启用 Windows 高 DPI 支持
if(MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE
        -mwindows
    )
endif()

# ==================== Windows 资源文件 ====================
# 为 exe 嵌入版本信息和图标（可选）
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/src/resources/version.rc")
    target_sources(${PROJECT_NAME} PRIVATE
        src/resources/version.rc
    )
endif()

# ==================== 安装规则（可选） ====================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION .
)
