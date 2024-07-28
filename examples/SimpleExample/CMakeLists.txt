cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(SIMPLE_EXAMPLE)

add_compile_options(-fPIC)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SIMPLE_EXAMPLE
               main.cpp)



find_package(PkgConfig)

pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0>=1.4)
pkg_search_module(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.4)


include_directories(RTSP_SERVER /home/jared/gstreamer-rtsp-ssl-example)
include_directories(RTSP_SERVER /usr/local/include/gstreamer-1.0/)
include_directories(RTSP_SERVER /usr/include)
include_directories(RTSP_SERVER /usr/include/opencv4)
include_directories(RTSP_SERVER /usr/include/aarch64-linux-gnu)
include_directories(${GLIB_INCLUDE_DIRS}
                    ${GSTREAMER_INCLUDE_DIRS})






link_directories("/usr/lib/aarch64-linux-gnu")
link_directories(${GLIB_LIBRARY_DIRS}
                 ${GSTREAMER_LIBRARY_DIRS})


target_link_libraries(RTSP_SERVER stdc++)
target_link_libraries(RTSP_SERVER stdc++fs)
target_link_libraries(RTSP_SERVER  "opencv_dnn")
target_link_libraries(RTSP_SERVER  "opencv_gapi")
target_link_libraries(RTSP_SERVER  "opencv_highgui")
target_link_libraries(RTSP_SERVER  "opencv_ml")
target_link_libraries(RTSP_SERVER  "opencv_objdetect")
target_link_libraries(RTSP_SERVER  "opencv_photo")
target_link_libraries(RTSP_SERVER  "opencv_stitching")
target_link_libraries(RTSP_SERVER  "opencv_video")
target_link_libraries(RTSP_SERVER  "opencv_calib3d")
target_link_libraries(RTSP_SERVER  "opencv_features2d")
target_link_libraries(RTSP_SERVER  "opencv_flann")
target_link_libraries(RTSP_SERVER  "opencv_videoio")
target_link_libraries(RTSP_SERVER  "opencv_imgcodecs")
target_link_libraries(RTSP_SERVER  "opencv_imgproc")
target_link_libraries(RTSP_SERVER  "opencv_core") 




target_link_libraries(SIMPLE_EXAMPLE
    PkgConfig::gstreamer
    PkgConfig::gstreamer-sdp
    PkgConfig::gstreamer-app
    PkgConfig::gstreamer-video
)





