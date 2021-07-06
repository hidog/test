cmake_minimum_required(VERSION 2.6)


# print message
message("c11 example")
message("root path =" ${CMAKE_SOURCE_DIR} )

# Project Name
set( PROJECT_NAME c11_test )
project(${PROJECT_NAME})

# 不設定C FLAG, 使用qtcreator的時候,會在 NULL的地方報錯
# 幾個c++11的設定方式
set (CMAKE_CXX_STANDARD 11)
#set( CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=gnu11" )
#set( CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=c11" )
#set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++11" )
#set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11" )


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
set( HEAD ${CMAKE_SOURCE_DIR}/../c11_sample.h )

# source 
set( SOURCE main.cpp 
            ${CMAKE_SOURCE_DIR}/../c11_sample.cpp )

# exec file
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )


