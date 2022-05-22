cmake_minimum_required (VERSION 3.8)

project ("TEST_OPENCV")

set(OpenCV_DIR D:/OpenCV4.5/opencv/build)
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable (TEST_OPENCV "TEST_OPENCV.cpp" "TEST_OPENCV.h" "All_pack.h")

target_link_libraries( TEST_OPENCV ${OpenCV_LIBS} )
