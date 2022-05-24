cmake_minimum_required(VERSION 3.14)

# Project Name
set( PROJECT_NAME gtest_example )
project(${PROJECT_NAME})

find_package(GTest REQUIRED)

# include path 
set( INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../sample/ )
include_directories( ${INCLUDE_PATH} )

# output path
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )


# head
set( HEAD ${CMAKE_SOURCE_DIR}/../sample/sample1.h )

# source 
set( SOURCE main.cc
            ${CMAKE_SOURCE_DIR}/../sample/sample1.cc
            ${CMAKE_SOURCE_DIR}/../sample/sample1_unittest.cc )

# exec file
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )

target_link_libraries( ${PROJECG_NAME} gtest_main)
