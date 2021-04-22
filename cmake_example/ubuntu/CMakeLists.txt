cmake_minimum_required(VERSION 3.6)

# print message
message("cmake ubuntu test project")
message("root path =" ${CMAKE_SOURCE_DIR} )

# Project Name
set( PROJECT_NAME cmake_ub_test )
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





# output path
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

# 不指定的話,編譯成Release
if( "${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)
endif()

# preprocessor define
set( DEFINE -DHIDOG_DEFINE )
if( UNIX )
	message("define unix")
    set( DEFINE ${DEFINE} -DUNIX )
endif( UNIX )
add_definitions( ${DEFINE} )

# include path
set( INCLUDE_PATH /home/hidog/code/test/cmake_example/ubuntu/so 
                  /home/hidog/code/test/cmake_example/ubuntu/a  )
include_directories( ${INCLUDE_PATH} )
message("可以用 CMAKE_SOURCE_DIR. " ${CMAKE_SOURCE_DIR} )

# library path 
set( LIB_PATH /home/hidog/code/test/cmake_example/ubuntu/a )
link_directories( ${LIB_PATH} )

# source 
set( SOURCE main.cpp )

# 
add_executable( ${PROJECT_NAME} ${SOURCE} )

# 指定路徑連結so的範例
target_link_libraries( ${PROJECT_NAME} /home/hidog/code/test/cmake_example/ubuntu/so/libtest.so )

# 自己製作出來的 libtest2.a
target_link_libraries( ${PROJECT_NAME} test2 )

