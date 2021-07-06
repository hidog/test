cmake_minimum_required(VERSION 2.6)


# print message
message("c11 example")
message("root path =" ${CMAKE_SOURCE_DIR} )

# Project Name
set( PROJECT_NAME c11_test )
project(${PROJECT_NAME})



# include path 
set( INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../src/ )
include_directories( ${INCLUDE_PATH} )

# output path
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

# set default release build
if( "${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)
endif()

# preprocessor define
message("define unix")
set( DEFINE ${DEFINE} -DUNIX )
add_definitions( ${DEFINE} ) 

# head
set( HEAD ${CMAKE_SOURCE_DIR}/../src/linklist.h )

# source 
set( SOURCE main.cpp 
            ${CMAKE_SOURCE_DIR}/../src/linklist.cpp )

# exec file
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )


