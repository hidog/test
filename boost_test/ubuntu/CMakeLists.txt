cmake_minimum_required(VERSION 2.6)

# sub project
#include(${CMAKE_SOURCE_DIR}/cmake/numeric_vector.cmake)


# print message
message("cmake test project")
message("root path =" ${CMAKE_SOURCE_DIR} )

# Project Name
set( PROJECT_NAME boost_test )
project(${PROJECT_NAME})

# output path
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

# preprocessor define
if( MSVC )
	message("define MSVC")
	set( DEFINE ${DEFINE} -DMSVC 
	                      -DWIN32 )
elseif( XCODE )
	message("define xcode")
	set( DEFINE ${DEFINE} -DMACOS )
elseif( UNIX )
        message("define unix")
        set( DEFINE ${DEFINE} -DUNIX )
endif( MSVC )
add_definitions( ${DEFINE} ) 

# include path
set( INCLUDE_PATH /home/hidog/code/test/boost
                  ${CMAKE_SOURCE_DIR}/../src/ )
include_directories( ${INCLUDE_PATH} )

# library link 
#set(LIB_PATH /usr/local/lib/)
#link_directories( ${LIB_PATH} )

# source 
set( SOURCE main.cpp 
            ${CMAKE_SOURCE_DIR}/../src/algorithm.cpp
            ${CMAKE_SOURCE_DIR}/../src/container.cpp
            ${CMAKE_SOURCE_DIR}/../src/hardware.cpp
            ${CMAKE_SOURCE_DIR}/../src/json.cpp 
            ${CMAKE_SOURCE_DIR}/../src/thread.cpp 
            ${CMAKE_SOURCE_DIR}/../src/uuid.cpp )

# head
set( HEAD   ${CMAKE_SOURCE_DIR}/../src/algorithm.h
            ${CMAKE_SOURCE_DIR}/../src/container.h
            ${CMAKE_SOURCE_DIR}/../src/hardware.h
            ${CMAKE_SOURCE_DIR}/../src/json.h
            ${CMAKE_SOURCE_DIR}/../src/thread.h
            ${CMAKE_SOURCE_DIR}/../src/uuid.h )


# exec file
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )

find_package( Boost REQUIRED COMPONENTS
                    system thread )
                    
target_link_libraries( ${PROJECT_NAME} Boost::system Boost::thread pthread )

#target_link_libraries( ${PROJECT_NAME} boost_system boost_thread pthread)

