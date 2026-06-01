cmake_minimum_required(VERSION 3.16)
project(http-server) 
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED)


set(SOURCES
    src/main.cpp
    src/httpserver.cpp
    src/threadpool.cpp
    src/file_cache.h
)

set(HEADERS
    src/util.h
    src/httpserver.h
    src/http_util.h
    src/http_util.cpp
    src/threadpool.h
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)