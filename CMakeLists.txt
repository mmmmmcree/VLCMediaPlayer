cmake_minimum_required(VERSION 3.5) # 指定CMake的最小版本为3.5
project(vlc LANGUAGES CXX) # 定义项目名称和使用的编程语言
 
# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# set(CMAKE_PREFIX_PATH "E:/Qt/6.6.3/mingw_64")
set(CMAKE_PREFIX_PATH "E:/Qt/6.6.3/msvc2019_64")

# 开启Qt的自动处理功能
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt6中的Widgets模块
find_package(Qt6 COMPONENTS Widgets Network Sql WebEngineWidgets REQUIRED)

# 包含VLC SDK的头文件目录
include_directories(./vlc-3.0.20/sdk/include)

# 使用find_library查找VLC库文件的确切路径
find_library(LIBVLC_LIBRARY NAMES libvlc PATHS ./vlc-3.0.20/sdk/lib NO_DEFAULT_PATH)
find_library(LIBVLCCORE_LIBRARY NAMES libvlccore PATHS ./vlc-3.0.20/sdk/lib NO_DEFAULT_PATH)

if(NOT LIBVLC_LIBRARY OR NOT LIBVLCCORE_LIBRARY)
    message(FATAL_ERROR "VLC libraries not found")
endif()

# 添加源代码目录
aux_source_directory(./src srcs)

# 设置字符编码为UTF-8（特别针对MSVC编译器）
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(${PROJECT_NAME}
    WIN32
    ${srcs}
    res/res.qrc
)
# 链接Qt Widgets和VLC库
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network Qt6::Sql Qt6::WebEngineWidgets ${LIBVLC_LIBRARY} ${LIBVLCCORE_LIBRARY})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/vlc-3.0.20/libvlc.dll"
    "${CMAKE_SOURCE_DIR}/vlc-3.0.20/libvlccore.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)