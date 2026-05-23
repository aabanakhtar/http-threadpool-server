cmake_minimum_required(VERSION 3.16)

project(http-server) 
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED)


set(SOURCES
    src/main.cpp
    src/util.h
    src/http.cpp
    src/http.h
    src/threadpool.h
    src/threadpool.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
