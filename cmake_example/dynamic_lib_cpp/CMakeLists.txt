# https://zh.m.wikibooks.org/zh-tw/CMake_%E5%85%A5%E9%96%80/%E5%BB%BA%E7%BD%AE%E8%88%87%E9%80%A3%E7%B5%90%E7%A8%8B%E5%BC%8F%E5%BA%AB
# 參考這個網址,增加其他範例

cmake_minimum_required(VERSION 3.6)

# print message
message("cmake dynamic lib cpp project ")
message("root path =" ${CMAKE_SOURCE_DIR} )

# Project Name
set( PROJECT_NAME dynamic_lib_cpp )
project(${PROJECT_NAME})

# output path
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dynamic/ )

# preprocessor define
set( DEFINE -DSTATIC_LIB_CPP )
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
set( SOURCE static_lib_1.cpp )

# head
set( HEAD static_lib_1.h )

# 
add_library( ${PROJECT_NAME} SHARED ${SOURCE} )


