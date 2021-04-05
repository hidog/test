cmake_minimum_required(VERSION 3.5)

project(smart_point LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( HEAD test.h
          smart_point.h )

set( SOURCE test.cpp
            smart_point.cpp )

add_executable(smart_point main.cpp ${SOURCE} ${HEAD} )
