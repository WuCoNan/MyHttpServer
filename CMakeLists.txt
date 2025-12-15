cmake_minimum_required(VERSION 3.10)
project(MyHttpServer)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

file(GLOB_RECURSE SRC 
    ${PROJECT_SOURCE_DIR}/gomokuServer/src/*.cpp
    ${PROJECT_SOURCE_DIR}/gomokuServer/src/handlers/*.cpp
    ${PROJECT_SOURCE_DIR}/http/src/*.cpp
    ${PROJECT_SOURCE_DIR}/muduo/src/*.cpp
    ${PROJECT_SOURCE_DIR}/router/src/*.cpp
    ${PROJECT_SOURCE_DIR}/sql/src/*.cpp
    ${PROJECT_SOURCE_DIR}/ssl/src/*.cpp
    )

# 添加可执行文件 - 使用明确的目标名称
add_executable(MyhttpServer ${SRC})
# 包含头文件目录
target_include_directories(MyhttpServer PUBLIC 
                          ${PROJECT_SOURCE_DIR}/gomokuServer/include
                          ${PROJECT_SOURCE_DIR}/gomokuServer/include/handlers
                          ${PROJECT_SOURCE_DIR}/http/include
                          ${PROJECT_SOURCE_DIR}/muduo/include
                          ${PROJECT_SOURCE_DIR}/router/include
                          ${PROJECT_SOURCE_DIR}/sql/include
                          ${PROJECT_SOURCE_DIR}/ssl/include        
                          )



set(CMAKE_BUILD_TYPE "Release")






# 链接 pthread 库
target_link_libraries(MyhttpServer pthread)
target_link_libraries(MyhttpServer /usr/lib/x86_64-linux-gnu/libmysqlclient.so)
find_package(OpenSSL REQUIRED)
target_link_libraries(MyhttpServer OpenSSL::SSL OpenSSL::Crypto)