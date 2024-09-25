cmake_minimum_required(VERSION 3.5.0)
project(dicom-image-reader VERSION 0.1.0 LANGUAGES C CXX)

set(OpenCV_DIR "C:/opencv/build")
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(dicom-image-reader main.cpp)
target_link_libraries( dicom-image-reader ${OpenCV_LIBS} )

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
