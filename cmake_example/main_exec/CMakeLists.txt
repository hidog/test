cmake_minimum_required(VERSION 3.6)


# print message
message("cmake exec test project")
message("root path =" ${CMAKE_SOURCE_DIR} )


# Project Name
set( PROJECT_NAME cmake_exec )
project(${PROJECT_NAME})


# 看到但沒測過, warning的設置.
# set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
# set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")



if( UNIX )
    # 不指定的話,UNIX預設編譯成Debug
    # 這邊不可以用MATCHES, 要用STREQUAL.
	if( "${CMAKE_BUILD_TYPE}" STREQUAL "") 
		set(CMAKE_BUILD_TYPE Release)
	endif()
endif( UNIX )


# 沒設定的情況下會自動增加 NDEBUG 到 Releae build底下. 
# Debug 不會增加 DEBUG _DEBUG macro
# mac環境下進行底下測試 
# 1. 沒設置 CMAKE_C_FLAGS_DEBUG, CMAKE_CXX_FLAGS_DEBUG, 能用中斷點debug
# 2. 設置 set( CMAKE_C_FLAGS_DEBUG "-DDEBUG -D_DEBUG -DTEST_DEBUG" ), 無法用中斷點debug
# 3. 增加 -ggdb, 可進行debug. set( CMAKE_C_FLAGS_DEBUG "-DDEBUG -D_DEBUG -DTEST_DEBUG -O0 -ggdb" )
# 最後找到可用的語法  set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -D_DEBUG -DTEST_DEBUG" )
set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -D_DEBUG -DTEST_DEBUG" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -D_DEBUG -DTEST_DEBUG" )


# Release build可以用底下的方式啟動ggdb.
# 這個指令出錯,跑出多餘的 ';'    =>   set( CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE} "-DNDEBUG -DTEST_RELEASE -ggdb" )
#set( CMAKE_C_FLAGS_RELEASE "-DNDEBUG -DTEST_RELEASE -ggdb" )
#set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -DTEST_RELEASE -ggdb" )



# output path
if( UNIX )
    if( CMAKE_BUILD_TYPE MATCHES Debug )
	    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug )
	else()
    	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
	endif()
endif( UNIX )


# preprocessor define
set( DEFINE -DMAIN_EXEC=55113 )
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
set( INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../static_lib/
			      ${CMAKE_SOURCE_DIR}/../dynamic_lib )
include_directories( ${INCLUDE_PATH} )
# message("可以用 CMAKE_SOURCE_DIR. " ${CMAKE_SOURCE_DIR} )


# library path
if( MSVC )
	# MSVC 環境下會自動增加debug/release分類進去.
	set( LIB_PATH 
		 ${CMAKE_SOURCE_DIR}/../static_lib/build/
		 ${CMAKE_SOURCE_DIR}/../dynamic_lib/build/ )
elseif( XCODE )
	# xcode環境同樣會自動搜尋到debug/release. 預設編譯輸出路徑是lib
	set( LIB_PATH 
		 ${CMAKE_SOURCE_DIR}/../static_lib/build/lib/
		 ${CMAKE_SOURCE_DIR}/../dynamic_lib/build/lib/ )
elseif( UNIX )
    if( CMAKE_BUILD_TYPE MATCHES Debug )
    	set( LIB_PATH ${CMAKE_SOURCE_DIR}/../static_lib/debug/
                      ${CMAKE_SOURCE_DIR}/../dynamic_lib/debug/ )
    else()
        set( LIB_PATH ${CMAKE_SOURCE_DIR}/../static_lib/lib/
                      ${CMAKE_SOURCE_DIR}/../dynamic_lib/lib/ )
    endif()
endif( MSVC )
link_directories( ${LIB_PATH} )


# source 
set( SOURCE main.cpp 
            static.cpp
			dynamic.cpp 
			explicit_load.cpp )
			
			
# head
set( HEAD static.h
          dynamic.h 
          explicit_load.h )


# 
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )



if( UNIX )
    # 底下是指定路徑連結so的範例, linux環境下可以用這個方式讀取指定路徑的 .so 檔案. 
    # windows無法用這個方式載入dll
    #target_link_libraries( ${PROJECT_NAME} static_lib dl /home/hidog/code/test/cmake_example/dynamic_lib/lib/libdynamic_lib.so )
    target_link_libraries( ${PROJECT_NAME} dynamic_lib static_lib dl )
    # dl use for explicit load so.
    # 測試結果, mac 自動增加 dl 進去專案.
    # mac 環境下需要複製dylib到執行檔目錄
    # UNIX環境不用copy .so 檔案
else()
    target_link_libraries( ${PROJECT_NAME} dynamic_lib static_lib )
endif()


