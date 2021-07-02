cmake_minimum_required(VERSION 3.6)


# print message
message("cmake static lib project ")
message("root path =" ${CMAKE_SOURCE_DIR} )


# Project Name
set( PROJECT_NAME static_lib )
project(${PROJECT_NAME})


# 沒設定的話, 預設是debug
if( UNIX )
    if( "${CMAKE_BUILD_TYPE}" STREQUAL "" )
        set(CMAKE_BUILD_TYPE Release)
    endif()
endif( UNIX )


#
set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -D_DEBUG -DTEST_DEBUG" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_DEBUG -DTEST_DEBUG" )


# output path
if( UNIX )
    if( CMAKE_BUILD_TYPE MATCHES Release )
        set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/ )
    else()
        set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug/ )
    endif()
endif( UNIX )


# preprocessor define
set( DEFINE -DSTATIC_LIB=3242 )
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
set( INCLUDE_PATH ${CMAKE_SOURCE_DIR}/ )
include_directories( ${INCLUDE_PATH} )


# source 
set( SOURCE static_lib_1.cpp
            static_lib_2.c
            static_lib_3.c )


# head
set( HEAD static_lib_1.h 
          static_lib_2.h 
          static_lib_3.h )


# 
add_library( ${PROJECT_NAME} STATIC ${HEAD} ${SOURCE} )


