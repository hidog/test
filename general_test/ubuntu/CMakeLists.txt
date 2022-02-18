cmake_minimum_required(VERSION 2.6)


# print message
message("general example")
message("root path =" ${CMAKE_SOURCE_DIR} )

# Project Name
set( PROJECT_NAME test_anything )
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
set( HEAD ${CMAKE_SOURCE_DIR}/../src/linklist.h
          ${CMAKE_SOURCE_DIR}/../src/scanf_regular.h
          ${CMAKE_SOURCE_DIR}/../src/macro.h
          ${CMAKE_SOURCE_DIR}/../src/pointer.h
          ${CMAKE_SOURCE_DIR}/../src/variable_arg.h
          ${CMAKE_SOURCE_DIR}/../src/other_test.h
          ${CMAKE_SOURCE_DIR}/../src/template.h
          ${CMAKE_SOURCE_DIR}/../src/typeid.h
          ${CMAKE_SOURCE_DIR}/../src/class/adv_array.h
          ${CMAKE_SOURCE_DIR}/../src/class/array.h
          ${CMAKE_SOURCE_DIR}/../src/class/inherit.h
          ${CMAKE_SOURCE_DIR}/../src/class/rule_three.h
          ${CMAKE_SOURCE_DIR}/../src/cast/cast_test.h
          ${CMAKE_SOURCE_DIR}/../src/cast/cast_obj.h )

# source 
set( SOURCE main.cpp 
            ${CMAKE_SOURCE_DIR}/../src/linklist.cpp
            ${CMAKE_SOURCE_DIR}/../src/scanf_regular.cpp
            ${CMAKE_SOURCE_DIR}/../src/macro.cpp
            ${CMAKE_SOURCE_DIR}/../src/pointer.cpp
            ${CMAKE_SOURCE_DIR}/../src/variable_arg.cpp
            ${CMAKE_SOURCE_DIR}/../src/other_test.cpp
            ${CMAKE_SOURCE_DIR}/../src/template.cpp
            ${CMAKE_SOURCE_DIR}/../src/typeid.cpp
            ${CMAKE_SOURCE_DIR}/../src/class/adv_array.cpp
            ${CMAKE_SOURCE_DIR}/../src/class/array.cpp
            ${CMAKE_SOURCE_DIR}/../src/class/inherit.cpp
            ${CMAKE_SOURCE_DIR}/../src/class/rule_three.cpp
            ${CMAKE_SOURCE_DIR}/../src/cast/cast_test.cpp
            ${CMAKE_SOURCE_DIR}/../src/cast/cast_obj.cpp )

# exec file
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )


