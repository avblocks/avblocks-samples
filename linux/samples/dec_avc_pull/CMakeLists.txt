cmake_minimum_required(VERSION 2.8.4)

set(Project_Name "dec_avc_pull")

set(CMAKE_VERBOSE_MAKEFILE true)

set(CMAKE_CONFIGURATION_TYPES "Debug32;Release32;Debug64;Release64" CACHE STRING "" FORCE)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release64 CACHE STRING "Choose the build type" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_SKIP_BUILD_RPATH  false)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH true) 
SET(CMAKE_INSTALL_RPATH "./")
#SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH false)

project(${Project_Name})

set(SOURCE_FILES 
	"${CMAKE_CURRENT_SOURCE_DIR}/dec_avc_pull.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/options.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/options.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/util.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/program_options.h"
)


include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# tip: use WARNING to trace message in CLion
#message(WARNING ${CMAKE_CXX_FLAGS_DEBUG32})

set(CMAKE_CXX_FLAGS_DEBUG32 "${CMAKE_CXX_FLAGS_DEBUG} -m32 ")
set(CMAKE_CXX_FLAGS_RELEASE32 "${CMAKE_CXX_FLAGS_RELEASE} -m32")

set(CMAKE_CXX_FLAGS_DEBUG64 "${CMAKE_CXX_FLAGS_DEBUG} -m64")
set(CMAKE_CXX_FLAGS_RELEASE64 "${CMAKE_CXX_FLAGS_RELEASE} -m64")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

if (CMAKE_BUILD_TYPE STREQUAL "Debug32" OR CMAKE_BUILD_TYPE STREQUAL "Release32")
    add_executable(${Project_Name}.app ${SOURCE_FILES})
    target_link_libraries(${Project_Name}.app AVBlocks)
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug64" OR CMAKE_BUILD_TYPE STREQUAL "Release64")
    add_executable(${Project_Name}64.app ${SOURCE_FILES})
    target_link_libraries(${Project_Name}64.app AVBlocks64)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug32" OR CMAKE_BUILD_TYPE STREQUAL "Release32")
