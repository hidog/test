cmake_minimum_required(VERSION 2.6)

set( PROJECT_NAME ffmpeg_example )
project(${PROJECT_NAME})

if( "${CMAKE_BUILD_TYPE}" STREQUAL "") 
	set(CMAKE_BUILD_TYPE Debug)
endif()

# preprocessor define
set( DEFINE ${DEFINE} -DUNIX )
add_definitions( ${DEFINE} )

# include path 
#set( INCLUDE_PATH ${CMAKE_SOURCE_DIR}/ffmpeg/include )
#include_directories( ${INCLUDE_PATH} )

# library path
#set( LIB_PATH ${CMAKE_SOURCE_DIR}/ffmpeg/lib/ )
#link_directories( ${LIB_PATH} )

set(ENV{PKG_CONFIG_PATH} ./ffmpeg/lib/pkgconfig)
find_package(PkgConfig REQUIRED)
pkg_check_modules(
    ffmpeg REQUIRED IMPORTED_TARGET     
    libavdevice
    libavfilter
    libavformat
    libavcodec
    libswresample
    libswscale
    libavutil
)

#include_directories(${AVFORMAT_INCLUDE_DIRS} ${AVUTIL_INCLUDE_DIRS})
#link_directories(${AVFORMAT_LIBRARY_DIRS} ${AVUTIL_LIBRARY_DIRS})

# source 
set( SOURCE ffplay.c )

# head
set( HEAD )

add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )
target_link_libraries( ${PROJECT_NAME} PRIVATE PkgConfig::ffmpeg )
