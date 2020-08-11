cmake_minimum_required(VERSION 3.0)

set(EXAMPLE_NAME ${PROJECT_NAME}_qt_test)

find_package(Qt5 COMPONENTS Core REQUIRED)

add_executable(${EXAMPLE_NAME}
	test_tinymat.cpp
)

if(TinyMAT_BUILD_STATIC_LIBS)
    target_link_libraries(${EXAMPLE_NAME} TinyMAT)
elseif(TinyMAT_BUILD_SHARED_LIBS)
    target_link_libraries(${EXAMPLE_NAME} TinyMATShared)
endif()
target_link_libraries(${EXAMPLE_NAME} Qt5::Core)


# Installation
install(TARGETS ${EXAMPLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/test_qt_read.m DESTINATION ${CMAKE_INSTALL_BINDIR})

#Installation of Qt DLLs on Windows
include(tinymat_deployqt)
tinymat_deployqt(${EXAMPLE_NAME})
