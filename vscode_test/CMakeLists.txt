cmake_minimum_required(VERSION 2.8)


# print message
message("vscode test")
message("root path =" ${CMAKE_SOURCE_DIR} )

# Project Name
set( PROJECT_NAME vscode_test )
project(${PROJECT_NAME})

# 不設定C FLAG, 使用qtcreator的時候,會在 NULL的地方報錯
# 設 c11, c++11, 使用 addrinfo 的時候會跳出 storage size of ‘hints’ isn’t known 的錯誤
# set( CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=gnu11" )
# set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++11" )

# output path
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

# set default release build
#if( "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
#    set(CMAKE_BUILD_TYPE Release)
#endif()

# preprocessor define
message("define unix")
set( DEFINE ${DEFINE} -DUNIX )
add_definitions( ${DEFINE} ) 

# head
set( HEAD ${CMAKE_SOURCE_DIR}/test.h )

# source 
set( SOURCE main.cpp 
            ${CMAKE_SOURCE_DIR}/test.cpp )

# exec file
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )


