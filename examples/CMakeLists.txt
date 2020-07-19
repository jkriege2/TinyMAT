cmake_minimum_required(VERSION 3.10)

#default test (C++ stdlib-only)
add_subdirectory(basic_test)

#optional test: using Qt framework
if (${Qt5_FOUND})
        add_subdirectory(test_qt)
endif()

#optional test: using OpenCV framework
if (${OpenCV_FOUND})
        add_subdirectory(test_opencv)
endif()
