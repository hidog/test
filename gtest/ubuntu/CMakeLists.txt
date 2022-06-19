cmake_minimum_required(VERSION 3.14)



# Project Name
set( PROJECT_NAME gtest_example )
project(${PROJECT_NAME})



# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)






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
            ${CMAKE_SOURCE_DIR}/../sample/sample1_unittest.cc 
	    ${CMAKE_SOURCE_DIR}/../sample/std_vector_test.cc )

# exec file
enable_testing()
#find_package(GTest REQUIRED)

add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )
target_link_libraries( ${PROJECT_NAME} gtest gtest_main pthread)

#add_test(FactorialTest)

include(GoogleTest)
gtest_discover_tests( ${PROJECT_NAME} )
