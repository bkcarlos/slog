cmake_minimum_required(VERSION 3.16)
project(slog_test CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 定义 spdlog 的源文件和头文件路径
set(SPDLOG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../spdlog/include)
message(STATUS "spdlog include directory: ${SPDLOG_INCLUDE_DIR}")
include_directories(${SPDLOG_INCLUDE_DIR})
set(SLOG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../include)
message(STATUS "slog include directory: ${SLOG_INCLUDE_DIR}")
include_directories(${SLOG_INCLUDE_DIR})

set(SPDLOG_SRC_DIR ${CMAKE_SOURCE_DIR}/spdlog/src)

# 定义 spdlog 源文件
file(GLOB SPDLOG_SRC ${SPDLOG_SRC_DIR}/*.cpp)

# 打印 spdlog 源文件路径（可选）
message(STATUS "spdlog source files: ${SPDLOG_SRC}")

# 添加 spdlog 头文件路径（更推荐使用目标范围的方式）
add_executable(${PROJECT_NAME}_spdlog spdlog_test.cpp ${SPDLOG_SRC})
target_include_directories(${PROJECT_NAME}_spdlog PRIVATE ${SPDLOG_INCLUDE_DIR})

add_executable(${PROJECT_NAME}_slog wrapping_test.cpp ../src/slog.cpp ${SPDLOG_SRC})
target_include_directories(${PROJECT_NAME}_slog PRIVATE ${SPDLOG_INCLUDE_DIR})

# 可选：为目标设置特定编译选项
target_compile_options(${PROJECT_NAME}_spdlog PRIVATE -Wall -Wextra -Werror)
target_compile_options(${PROJECT_NAME}_slog PRIVATE -Wall -Wextra -Werror)