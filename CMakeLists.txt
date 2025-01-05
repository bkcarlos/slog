cmake_minimum_required(VERSION 3.15)

# 项目名称和版本
project(slog VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置调试模式和调试信息分离选项
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wl,--build-id")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Wl,--build-id")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -Wl,--build-id")

include(GNUInstallDirs)

# 添加 spdlog 子目录
set(SPDLOG_BUILD_PIC ON)  # 构建 spdlog 为位置无关代码
add_subdirectory(spdlog)

# 定义头文件和源文件路径
set(SPDLOG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/spdlog/include)
set(SLOG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SLOG_SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# 打印路径信息
message(STATUS "spdlog include directory: ${SPDLOG_INCLUDE_DIR}")
message(STATUS "slog include directory: ${SLOG_INCLUDE_DIR}")

# 查找 objcopy
find_program(CMAKE_OBJCOPY NAMES objcopy)
if(NOT CMAKE_OBJCOPY)
    message(FATAL_ERROR "objcopy not found! Ensure GNU binutils is installed.")
endif()

# 添加 slog 库（共享库）
add_library(${PROJECT_NAME} SHARED ${SLOG_SRC_DIR}/slog.cpp)

# 设置包含路径和库依赖
target_include_directories(${PROJECT_NAME} PUBLIC ${SLOG_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)

# 添加分离调试信息命令
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} --only-keep-debug $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE:${PROJECT_NAME}>.debug
    COMMAND ${CMAKE_OBJCOPY} --strip-debug $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE:${PROJECT_NAME}>.debug $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Separating debug symbols from ${PROJECT_NAME}"
)

# 设置输出的共享库名称
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "slog")

# 调试和发布模式设置
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_BUILD=1)
    message(STATUS "Building in Debug mode")
else()
    message(STATUS "Building in Release mode")
endif()

# 安装共享库，头文件和调试符号
# 安装共享库
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 安装头文件
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/spdlog/include/ DESTINATION include)
message(STATUS "Install directory: ${CMAKE_INSTALL_INCLUDEDIR}")

# 安装调试符号
install(FILES $<TARGET_FILE:${PROJECT_NAME}>.debug
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# add_subdirectory(test)