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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/modules)

add_definitions(-DGDRIFT_ROOT_DIR)
add_definitions(-DRESTBED_ROOT_DIR)

#find_package(Gdrift REQUIRED)
#if(NOT GDRIFT_FOUND)
#   message(FATAL_ERROR "Package libgdrift not found")
#endif(NOT GDRIFT_FOUND)

#find_package(Restbed REQUIRED)
#if(NOT RESTBED_FOUND)
#   message(FATAL_ERROR "Package restbed not found")
#endif(NOT RESTBED_FOUND)

#file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/include/*.cpp")
#link_directories(${GDRIFT_LIBRARY} ${MONGOCXX_LIBRARY} ${BSONCXX_LIBRARY} ${RESTBED_LIBRARY})
#include_directories(${CMAKE_SOURCE_DIR}/include )

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_library(libgdrift3 SHARED ${SOURCES})

#file(GLOB controller_SOURCES "${CMAKE_SOURCE_DIR}/controller/src/*.cc" ${SOURCES})
#include_directories(gdrift_service_controller ${CMAKE_SOURCE_DIR}/controller/include)
#add_executable(gdrift_service_controller ${controller_SOURCES})
#target_link_libraries(gdrift_service_controller mongocxx bsoncxx restbed gdrift++ ssl crypto pthread boost_thread boost_system curl)

#add_executable(prepare_data  "${CMAKE_SOURCE_DIR}/test/statistics/prepare_data.cc")
#target_link_libraries(prepare_data mongocxx bsoncxx restbed gdrift++ ssl crypto pthread boost_thread boost_system curl)

add_executable(construction_test "${CMAKE_SOURCE_DIR}/src/construction_test.cpp")
target_link_libraries(construction_test pthread)

link_directories(${CMAKE_BINARY_DIR})
target_link_libraries(construction_test libgdrift3)

#if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#    set (CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "default install path" FORCE )
#endif()

#install(TARGETS gdrift_service_scheduler gdrift_service_controller gdrift_service_results gdrift_service_analyzer DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)



