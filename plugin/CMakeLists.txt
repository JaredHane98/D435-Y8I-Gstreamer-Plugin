cmake_minimum_required(VERSION 3.5)

project(rsdeinterlace)
set(target gstrsdeinterlace)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()



set(LIBRARY_DEST "/usr/lib/aarch64-linux-gnu/gstreamer-1.0" CACHE PATH "Built Library destination")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++14 -Wno-deprecated-declarations")


set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SRC_DIR ${PROJECT_SOURCE_DIR}/src)


find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0 >= 1.4)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0 >= 1.4)
pkg_search_module(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0 >= 1.4)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0 >= 1.4)


include_directories(${GSTREAMER_INCLUDE_DIRS}
                    ${PROJECT_INCLUDE_DIR})


add_library(${target} SHARED ${PROJECT_SRC_DIR}/gstrsdeinterlace.cpp ${PROJECT_INCLUDE_DIR}/gstrsdeinterlace.hpp)


target_link_libraries(${target} 
                      PUBLIC
                      PkgConfig::gstreamer
                      PkgConfig::gstreamer-sdp
                      PkgConfig::gstreamer-app
                      PkgConfig::gstreamer-video)


find_path(GSTREAMER_INCLUDE_INSTALL_PATH 
          NAMES gstreamer-1.0
          PATH_SUFFIXES include)


message("LIBRARY INSTALL PATH: " ${LIBRARY_DEST})


if(NOT GSTREAMER_INCLUDE_INSTALL_PATH)
    message(FATAL_ERROR "FAILED TO FIND INCLUDE INSTALL DIRECTORY")
else()
    message("INCLUDE INSTALL PATH: " "${GSTREAMER_INCLUDE_INSTALL_PATH}/gstreamer-1.0/video")
endif()



set_target_properties(${target}  PROPERTIES PUBLIC_HEADER "${PROJECT_INCLUDE_DIR}/gstrsdeinterlace.hpp")

install(TARGETS ${target} 
        LIBRARY DESTINATION ${LIBRARY_DEST}
        PUBLIC_HEADER DESTINATION ${GSTREAMER_INCLUDE_INSTALL_PATH}/gstreamer-1.0/gst/video)
