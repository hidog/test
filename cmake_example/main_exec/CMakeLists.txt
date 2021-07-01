cmake_minimum_required(VERSION 3.6)


# print message
message("cmake exec test project")
message("root path =" ${CMAKE_SOURCE_DIR} )


# Project Name
set( PROJECT_NAME cmake_exec )
project(${PROJECT_NAME})




# 參考這個網址  設置一些flag的作法
# https://stackoverflow.com/questions/10085945/set-cflags-and-cxxflags-options-using-cmake/41843596   
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")
# set(CMAKE_BUILD_TYPE DEBUG)
# set(CMAKE_C_FLAGS "-O0 -ggdb")
# set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb")
# set(CMAKE_C_FLAGS_RELEASE "-O0 -ggdb")
# set(CMAKE_CXX_FLAGS "-O0 -ggdb")
# set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
# set(CMAKE_CXX_FLAGS_RELEASE "-O0 -ggdb")

# 看到但沒測過
# set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
# set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")

# https://www.796t.com/article.php?id=49714
# 參考這篇網誌的cmake設置	



# output path
if( UNIX )
	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
endif( UNIX )


if( UNIX )
# 不指定的話,UNIX預設編譯成Release
	if( "${CMAKE_BUILD_TYPE}" STREQUAL "")
		set(CMAKE_BUILD_TYPE Release)
	endif()
endif( UNIX )


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
if( MSVC )
	set( INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../static_lib/
					  ${CMAKE_SOURCE_DIR}/../dynamic_lib )
	include_directories( ${INCLUDE_PATH} )
elseif( UNIX )
	set( INCLUDE_PATH /home/hidog/code/test/cmake_example/ubuntu/so 
                      /home/hidog/code/test/cmake_example/ubuntu/a  )
endif( MSVC )
# message("可以用 CMAKE_SOURCE_DIR. " ${CMAKE_SOURCE_DIR} )


# library path
if( MSVC )
	# MSVC 環境下會自動增加debug/release分類進去.
	set( LIB_PATH 
		 ${CMAKE_SOURCE_DIR}/../static_lib/build/
		 ${CMAKE_SOURCE_DIR}/../dynamic_lib/build/ )
elseif( UNIX )
	set( LIB_PATH /home/hidog/code/test/cmake_example/ubuntu/a )
endif( MSVC )
link_directories( ${LIB_PATH} )


# source 
set( SOURCE main.cpp 
            static.cpp
			dynamic.cpp )
			
			
# head
set( HEAD static.h
          dynamic.h )


# 
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )


# 指定路徑連結so的範例
#target_link_libraries( ${PROJECT_NAME} /home/hidog/code/test/cmake_example/ubuntu/so/libtest.so )
# windows似乎不能這樣, 研究一下這邊的語法
#target_link_libraries( ${PROJECT_NAME} D:/code/test/cmake_example/dynamic_lib_cpp/build/Debug/dynamic_lib.dll )


# 自己製作出來的 libtest2.a
target_link_libraries( ${PROJECT_NAME} dynamic_lib static_lib )

