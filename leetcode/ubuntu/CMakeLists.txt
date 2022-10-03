cmake_minimum_required(VERSION 2.6)


# print message
message("leetcode")
message("root path =" ${CMAKE_SOURCE_DIR} )

# Project Name
set( PROJECT_NAME leetcode )
project(${PROJECT_NAME})

set( CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=gnu11" )
set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++11" )

# output path
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

# include path
set( INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../src/ )
include_directories( ${INCLUDE_PATH} )

# preprocessor define
message("define unix")
set( DEFINE ${DEFINE} -DUNIX )
add_definitions( ${DEFINE} ) 

# head
set( HEAD ${CMAKE_SOURCE_DIR}/../src/0/p1.h
          ${CMAKE_SOURCE_DIR}/../src/0/p2.h
          ${CMAKE_SOURCE_DIR}/../src/0/p3.h
          ${CMAKE_SOURCE_DIR}/../src/0/p4.h
          ${CMAKE_SOURCE_DIR}/../src/0/p9.h
          ${CMAKE_SOURCE_DIR}/../src/0/p15.h
          ${CMAKE_SOURCE_DIR}/../src/0/p33.h          
          ${CMAKE_SOURCE_DIR}/../src/0/p95.h
          ${CMAKE_SOURCE_DIR}/../src/0/p96.h
          ${CMAKE_SOURCE_DIR}/../src/1/p169.h
          ${CMAKE_SOURCE_DIR}/../src/6/p669.h
          ${CMAKE_SOURCE_DIR}/../src/8/p895.h
          ${CMAKE_SOURCE_DIR}/../src/10/p1007.h
)

# source 
set( SOURCE main.cpp 
            ${CMAKE_SOURCE_DIR}/../src/0/p1.cpp
            ${CMAKE_SOURCE_DIR}/../src/0/p2.cpp
	        ${CMAKE_SOURCE_DIR}/../src/0/p3.cpp
            ${CMAKE_SOURCE_DIR}/../src/0/p4.cpp
            ${CMAKE_SOURCE_DIR}/../src/0/p9.cpp
            ${CMAKE_SOURCE_DIR}/../src/0/p15.cpp
            ${CMAKE_SOURCE_DIR}/../src/0/p33.cpp      
            ${CMAKE_SOURCE_DIR}/../src/0/p95.cpp
            ${CMAKE_SOURCE_DIR}/../src/0/p96.cpp
            ${CMAKE_SOURCE_DIR}/../src/1/p169.cpp
            ${CMAKE_SOURCE_DIR}/../src/6/p669.cpp
            ${CMAKE_SOURCE_DIR}/../src/8/p895.cpp
            ${CMAKE_SOURCE_DIR}/../src/10/p1007.cpp
)

# exec file
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )


