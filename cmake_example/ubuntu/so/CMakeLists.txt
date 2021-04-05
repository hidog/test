# https://zh.m.wikibooks.org/zh-tw/CMake_%E5%85%A5%E9%96%80/%E5%BB%BA%E7%BD%AE%E8%88%87%E9%80%A3%E7%B5%90%E7%A8%8B%E5%BC%8F%E5%BA%AB
# 參考這個網址,增加其他範例

cmake_minimum_required(VERSION 3.6)

# print message
message("cmake so test project")
message("root path =" ${CMAKE_SOURCE_DIR} )

# Project Name
set( PROJECT_NAME test )
project(${PROJECT_NAME})

# source 
set( SOURCE test.cpp )

# head
set( HEAD test.h )

# 
add_library( ${PROJECT_NAME} SHARED ${SOURCE} )


