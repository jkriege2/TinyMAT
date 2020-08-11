cmake_minimum_required(VERSION 3.0)

set(EXAMPLE_NAME ${PROJECT_NAME}_test_opencv)

find_package(OpenCV REQUIRED )
message("OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")
message("OpenCV_LIBS         = ${OpenCV_LIBS}")
message("OpenCV_FOUND        = ${OpenCV_FOUND}")

add_executable(${EXAMPLE_NAME}
	test_opencv_tinymat.cpp
)

if(TinyMAT_BUILD_STATIC_LIBS)
    target_link_libraries(${EXAMPLE_NAME} PRIVATE TinyMAT)
elseif(TinyMAT_BUILD_SHARED_LIBS)
    target_link_libraries(${EXAMPLE_NAME} PRIVATE TinyMATShared)
endif()
target_include_directories(${EXAMPLE_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${EXAMPLE_NAME} PRIVATE ${OpenCV_LIBS})


# Installation
install(TARGETS ${EXAMPLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/test_opencv_qt_read.m DESTINATION ${CMAKE_INSTALL_BINDIR})

#Installation of Qt DLLs on Windows
include(tinymat_deployqt)
