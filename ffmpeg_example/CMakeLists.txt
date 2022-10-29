cmake_minimum_required(VERSION 2.6)

set( PROJECT_NAME ffmpeg_example )
project(${PROJECT_NAME})

if( "${CMAKE_BUILD_TYPE}" STREQUAL "") 
	set(CMAKE_BUILD_TYPE Debug)
endif()

# preprocessor define
set( DEFINE ${DEFINE} -DUNIX )
add_definitions( ${DEFINE} )

#find_package(SDL2 REQUIRED)

# include path 
set( INCLUDE_PATH ${CMAKE_SOURCE_DIR}/ffmpeg/include 
                  ${CMAKE_SOURCE_DIR}/ffmpeg-5.1.2 
                  ${SDL2_INCLUDE_DIRS} )
include_directories( ${INCLUDE_PATH} )

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

# library path
set( LIB_PATH ${CMAKE_SOURCE_DIR}/ffmpeg/lib/ )
link_directories( ${LIB_PATH} )

# source 
set( SOURCE ./ffmpeg-5.1.2/fftools/ffmpeg.c )

# head
set( HEAD )

add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )
target_link_libraries( ${PROJECT_NAME} PRIVATE PkgConfig::ffmpeg )
