cmake_minimum_required(VERSION 2.8.9)
project(libgdrift3)
set(CMAKE_BUILD_TYPE Release)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O3")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -O3")
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(construction_test "${CMAKE_SOURCE_DIR}/tests/construction_test.cpp")
add_executable(threads_test "${CMAKE_SOURCE_DIR}/tests/threads_test.cpp")
add_executable(threads_test_serialized "${CMAKE_SOURCE_DIR}/tests/threads_test_serialized.cpp")
add_executable(full_test "${CMAKE_SOURCE_DIR}/tests/full_test.cpp")
add_executable(generate_statistics "${CMAKE_SOURCE_DIR}/tests/generate_statistics.cpp")
add_executable(test_parallel "${CMAKE_SOURCE_DIR}/tests/test_parallel.cpp")

add_library(libgdrift3 SHARED ${SOURCES})
link_directories(${CMAKE_BINARY_DIR})

target_link_libraries(construction_test libgdrift3)
target_link_libraries(threads_test libgdrift3 pthread)
target_link_libraries(threads_test_serialized libgdrift3 pthread)
target_link_libraries(full_test libgdrift3 pthread)
target_link_libraries(test_parallel libgdrift3 pthread)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/usr/" CACHE PATH "default install path" FORCE )
endif()

install(TARGETS libgdrift3 DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(TARGETS construction_test DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libgdrift3 FILES_MATCHING PATTERN "*.h")

