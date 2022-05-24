# Project Name
set( PROJECT_NAME gtest_example )
project(${PROJECT_NAME})

# include path 
set( INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../sample/ )
include_directories( ${INCLUDE_PATH} )

# output path
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

# head
set( HEAD ${CMAKE_SOURCE_DIR}/../sample/sample1.h )

# source 
set( SOURCE main.cc
            ${CMAKE_SOURCE_DIR}/../sample/sample1.cc
            ${CMAKE_SOURCE_DIR}/../sample/sample1_unittest.cc )

# exec file
enable_testing()
find_package(GTest REQUIRED)

add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )
target_link_libraries( ${PROJECT_NAME} gtest gtest_main pthread)

#add_test(FactorialTest)
