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

# preprocessor define
message("define unix")
set( DEFINE ${DEFINE} -DUNIX )
add_definitions( ${DEFINE} ) 

# head
set( HEAD ${CMAKE_SOURCE_DIR}/../p1.h
          ${CMAKE_SOURCE_DIR}/../p6.h
          ${CMAKE_SOURCE_DIR}/../p8.h 
)

# source 
set( SOURCE main.cpp 
            ${CMAKE_SOURCE_DIR}/../p1.cpp 
            ${CMAKE_SOURCE_DIR}/../p6.cpp
	        ${CMAKE_SOURCE_DIR}/../p8.cpp 
)

# exec file
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )


