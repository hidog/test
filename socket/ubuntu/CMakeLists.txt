cmake_minimum_required(VERSION 2.6)


# print message
message("ubuntu socket example")
message("root path =" ${CMAKE_SOURCE_DIR} )

# Project Name
set( PROJECT_NAME socket_test )
project(${PROJECT_NAME})

# output path
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

# preprocessor define
message("define unix")
set( DEFINE ${DEFINE} -DUNIX )
add_definitions( ${DEFINE} ) 

# head
set( HEAD ${CMAKE_SOURCE_DIR}/../udp_example.h
          ${CMAKE_SOURCE_DIR}/../tool.h )

# source 
set( SOURCE main.cpp 
            ${CMAKE_SOURCE_DIR}/../udp_example.cpp 
            ${CMAKE_SOURCE_DIR}/../tool.cpp )

# exec file
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )


