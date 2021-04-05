cmake_minimum_required(VERSION 2.6)

# Project Name
set( PROJECT_NAME std_test )
project(${PROJECT_NAME})

if( "${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)
endif()

# preprocessor define
if( UNIX )
    message("define unix")
    set( DEFINE ${DEFINE} -DUNIX )
endif( UNIX )
add_definitions( ${DEFINE} ) 

# include path
set( INCLUDE_PATH /home/hidog/code/test/std )
include_directories( ${INCLUDE_PATH} )

# source 
set( SOURCE main.cpp
            ../thread.cpp
            ../macro.cpp )

# exec file
#add_compile_options("-Wall")
add_executable( ${PROJECT_NAME} ${SOURCE}  )
target_link_libraries( ${PROJECT_NAME} pthread)



