cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(project
        VERSION 0.1
        DESCRIPTION "buding! yyds_"
        LANGUAGES C CXX
        )

# Compiler flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -std=c++17 -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-attributes") #TODO: remove
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fPIC")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fPIC")
set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fPIC")

set(GCC_COVERAGE_LINK_FLAGS "-fPIC")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

# 设置 C++ 编译器标志
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fno-omit-frame-pointer -fno-optimize-sibling-calls")

# Output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Includes.
#变量的作用域是局部的，所以如果当前目录下的文件需要头文件，就需要在这个cmake中包含
#set(PINE_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
set(PINE_SRC_SOCKBASE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/SockBase/include)
set(PINE_SRC_HTTPBASE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/HttpBase/include)
include_directories(${PINE_SRC_SOCKBASE_INCLUDE_DIR})		#不加这个语句虽然可以cmake通过，但是无法编译主文件
include_directories(${PINE_SRC_HTTPBASE_INCLUDE_DIR})

#to run CMakeLists.txt file of /src
add_subdirectory(src)
#add_subdirectory(src/SockBase)	#不包含了，在src中包含了



file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
foreach (test_source ${TEST_SOURCES})
    # Create a human readable name.
    get_filename_component(pine_test_filename ${test_source} NAME)
    string(REPLACE ".cpp" "" pine_test_name ${pine_test_filename})

    # Add the test target separately and as part of "msake check-tests".
    add_executable(${pine_test_name} EXCLUDE_FROM_ALL ${test_source})

	target_link_libraries(${pine_test_name} sockbase_lib)				#在这里链接头文件的
	target_link_libraries(${pine_test_name} httpbase_lib)
    # Set test target properties and dependencies.
    set_target_properties(${pine_test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"		#输出路
        COMMAND ${pine_test_name}
    )

endforeach(test_source ${TEST_SOURCES})

