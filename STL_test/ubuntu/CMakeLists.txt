cmake_minimum_required(VERSION 2.6)

# Project Name
set( PROJECT_NAME STL_example )
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
set( INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../src )
include_directories( ${INCLUDE_PATH} )

# head
set( HEAD ../src/smart_pt/smart_point.h
          ../src/thread/thread_obj.h
          ../src/thread/thread_test.h
          ../src/c11_timer.h
          ../src/other_test.h
          ../src/regular_express.h
          ../src/random.h
          ../src/smart_pt/sp_class.h 
          ../src/container.h
          ../src/for_each.h
          ../src/my_array.h
          ../src/function.h
          ../src/atomic_test.h
          ../src/my_map_class.hpp )

# source 
set( SOURCE main.cpp
            ../src/smart_pt/smart_point.cpp
            ../src/thread/thread_obj.cpp
            ../src/thread/thread_test.cpp           
            ../src/c11_timer.cpp
            ../src/other_test.cpp
            ../src/regular_express.cpp
            ../src/random.cpp
            ../src/smart_pt/sp_class.cpp
            ../src/container.cpp
            ../src/for_each.cpp
            ../src/my_array.cpp
            ../src/function.cpp
            ../src/atomic_test.cpp )

# exec file
#add_compile_options("-Wall")
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )
target_link_libraries( ${PROJECT_NAME} pthread)



