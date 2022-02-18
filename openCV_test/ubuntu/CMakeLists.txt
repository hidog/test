cmake_minimum_required(VERSION 3.6)


# print message
message("opencv test project")
message("root path =" ${CMAKE_SOURCE_DIR} )


# Project Name
set( PROJECT_NAME openCV_exec )
project(${PROJECT_NAME})

#
if( "${CMAKE_BUILD_TYPE}" STREQUAL "" )
    set(CMAKE_BUILD_TYPE Debug )
endif()

# output path
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

# preprocessor define
set( DEFINE ${DEFINE} -DUNIX )
add_definitions( ${DEFINE} )

# include path 
set( INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../src/ )
include_directories( ${INCLUDE_PATH} )

# source 
set( SOURCE main.cpp
            ../src/base_test.cpp
            ../src/base_test_2.cpp
            ../src/filter_test.cpp
            ../src/tool.cpp
            ../src/improcess.cpp )
			
# head
set( HEAD ../src/base_test.h
          ../src/base_test_2.h
          ../src/filter_test.h
          ../src/tool.h
          ../src/improcess.h )

# 
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )

# PATHS 為指定位置. 看起來ubuntu20原本就會安裝opencv  (或是因為其他套件安裝)
# NO_DEFAULT_PATH 沒有這個,會搜尋到預設的路徑.
find_package( OpenCV REQUIRED PATHS "/home/hidog/code/opencv/install" NO_DEFAULT_PATH )
# find_package( OpenCV 4.5.4 REQUIRED ) 這個會找不到
#find_package( OpenCV 4.5.4 REQUIRED PATHS "/home/hidog/code/opencv/install" ) 也可以用這個指定版本.

#
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

