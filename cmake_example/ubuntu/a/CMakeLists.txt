cmake_minimum_required(VERSION 3.6)

# print message
message("cmake .a test project")
message("root path =" ${CMAKE_SOURCE_DIR} )

# Project Name
set( PROJECT_NAME test2 )
project(${PROJECT_NAME})

# source 
set( SOURCE test2.cpp )

# head
set( HEAD test2.h )

# 
add_library( ${PROJECT_NAME} STATIC ${SOURCE} )


