cmake_minimum_required(VERSION 2.6)


# print message
message("c11 example")
message("root path =" ${CMAKE_SOURCE_DIR} )

# Project Name
set( PROJECT_NAME c11_test )
project(${PROJECT_NAME})

# 不設定C FLAG, 使用qtcreator的時候,會在 NULL的地方報錯
# 幾個c++11的設定方式
# 使用 set (CMAKE_CXX_STANDARD 98), 會造成編譯錯誤
set (CMAKE_CXX_STANDARD 11)
#set( CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=gnu11" )
#set( CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=c11" )
set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++11" )
#set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11" )




# include path 
set( INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../src/ )
include_directories( ${INCLUDE_PATH} )




# output path
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

# set default release build
if( "${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)
endif()

# preprocessor define
message("define unix")
set( DEFINE ${DEFINE} -DUNIX )
add_definitions( ${DEFINE} ) 

# head
set( HEAD ${CMAKE_SOURCE_DIR}/../src/c11_constexpr.h
          ${CMAKE_SOURCE_DIR}/../src/c11_enum_class.h
          ${CMAKE_SOURCE_DIR}/../src/c11_lambda_op.h
          ${CMAKE_SOURCE_DIR}/../src/c11_other_test.h
          ${CMAKE_SOURCE_DIR}/../src/c11_using.h
          ${CMAKE_SOURCE_DIR}/../src/c11_range_for.h
          ${CMAKE_SOURCE_DIR}/../src/class/c11_class_dd.h
          ${CMAKE_SOURCE_DIR}/../src/class/c11_explicit_class.h
          ${CMAKE_SOURCE_DIR}/../src/class/c11_mutable_class.h
          ${CMAKE_SOURCE_DIR}/../src/class/c11_rule_five.h
          ${CMAKE_SOURCE_DIR}/../src/class/rule_five_class.h
          ${CMAKE_SOURCE_DIR}/../src/inherit/c11_final_class.h
          ${CMAKE_SOURCE_DIR}/../src/inherit/c11_virtual_inherit.h
          ${CMAKE_SOURCE_DIR}/../src/initial/c11_initial.h
          ${CMAKE_SOURCE_DIR}/../src/initial/initial_base.h
          ${CMAKE_SOURCE_DIR}/../src/initial/initial_obj.h
          ${CMAKE_SOURCE_DIR}/../src/LR_value/c11_LR_value.h
          ${CMAKE_SOURCE_DIR}/../src/LR_value/lr_func.h
          ${CMAKE_SOURCE_DIR}/../src/move/c11_move.h
          ${CMAKE_SOURCE_DIR}/../src/move/c11_swap.h
          ${CMAKE_SOURCE_DIR}/../src/move/move_class_1.h
          ${CMAKE_SOURCE_DIR}/../src/move/move_class_2.h
          ${CMAKE_SOURCE_DIR}/../src/move/move_class_3.h
          ${CMAKE_SOURCE_DIR}/../src/move/move_func.h
          ${CMAKE_SOURCE_DIR}/../src/perfect_forward/c11_perfect_forward.h
          ${CMAKE_SOURCE_DIR}/../src/perfect_forward/pf_class.h
          ${CMAKE_SOURCE_DIR}/../src/perfect_forward/pf_func.h
          ${CMAKE_SOURCE_DIR}/../src/template/c11_template_test.h
          ${CMAKE_SOURCE_DIR}/../src/template/template_array.h
          ${CMAKE_SOURCE_DIR}/../src/template/template_func.h )

# source 
set( SOURCE main.cpp 
            ${CMAKE_SOURCE_DIR}/../src/c11_constexpr.cpp
            ${CMAKE_SOURCE_DIR}/../src/c11_enum_class.cpp
            ${CMAKE_SOURCE_DIR}/../src/c11_lambda_op.cpp
            ${CMAKE_SOURCE_DIR}/../src/c11_other_test.cpp
            ${CMAKE_SOURCE_DIR}/../src/c11_using.cpp
            ${CMAKE_SOURCE_DIR}/../src/c11_range_for.cpp
            ${CMAKE_SOURCE_DIR}/../src/class/c11_class_dd.cpp
            ${CMAKE_SOURCE_DIR}/../src/class/c11_explicit_class.cpp
            ${CMAKE_SOURCE_DIR}/../src/class/c11_mutable_class.cpp
            ${CMAKE_SOURCE_DIR}/../src/class/c11_rule_five.cpp
            ${CMAKE_SOURCE_DIR}/../src/class/rule_five_class.cpp
            ${CMAKE_SOURCE_DIR}/../src/inherit/c11_final_class.cpp
            ${CMAKE_SOURCE_DIR}/../src/inherit/c11_virtual_inherit.cpp
            ${CMAKE_SOURCE_DIR}/../src/initial/c11_initial.cpp
            ${CMAKE_SOURCE_DIR}/../src/initial/initial_base.cpp
            ${CMAKE_SOURCE_DIR}/../src/initial/initial_obj.cpp
            ${CMAKE_SOURCE_DIR}/../src/LR_value/c11_LR_value.cpp
            ${CMAKE_SOURCE_DIR}/../src/LR_value/lr_func.cpp 
            ${CMAKE_SOURCE_DIR}/../src/move/c11_move.cpp
            ${CMAKE_SOURCE_DIR}/../src/move/c11_swap.cpp
            ${CMAKE_SOURCE_DIR}/../src/move/move_class_1.cpp
            ${CMAKE_SOURCE_DIR}/../src/move/move_class_2.cpp
            ${CMAKE_SOURCE_DIR}/../src/move/move_class_3.cpp
            ${CMAKE_SOURCE_DIR}/../src/move/move_func.cpp
            ${CMAKE_SOURCE_DIR}/../src/perfect_forward/c11_perfect_forward.cpp
            ${CMAKE_SOURCE_DIR}/../src/perfect_forward/pf_func.cpp
            ${CMAKE_SOURCE_DIR}/../src/template/c11_template_test.cpp
            ${CMAKE_SOURCE_DIR}/../src/template/template_array.cpp )

# exec file
add_executable( ${PROJECT_NAME} ${SOURCE} ${HEAD} )


