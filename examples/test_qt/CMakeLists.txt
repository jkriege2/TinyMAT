cmake_minimum_required(VERSION 3.10)

set(EXAMPLE_NAME ${PROJECT_NAME}_qt_test)

add_executable(${EXAMPLE_NAME}
	test_tinymat.cpp
)

target_link_libraries(${EXAMPLE_NAME} TinyMAT::TinyMAT)
target_link_libraries(${EXAMPLE_NAME} Qt${QT_VERSION_MAJOR}::Core)


# Installation
install(TARGETS ${EXAMPLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/test_qt_read.m DESTINATION ${CMAKE_INSTALL_BINDIR})

#Installation of Qt DLLs on Windows
include(tinymat_deployqt)
tinymat_deployqt(${EXAMPLE_NAME})
