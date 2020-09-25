cmake_minimum_required(VERSION 3.0.0)

project(cv_test_tri VERSION 0.1.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /home/tad/opencv_build/pkg3.4.1/share/OpenCV/)
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
 
include_directories(${OpenCV_INCLUDE_DIRS})
 

add_executable( rect_detect1 src/rect_detect1.cpp)
target_link_libraries(rect_detect1 ${OpenCV_LIBS})

add_executable( rect_detect2 src/rect_detect2.cpp)
target_link_libraries(rect_detect2 ${OpenCV_LIBS})



