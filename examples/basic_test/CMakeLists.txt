cmake_minimum_required(VERSION 3.10)

set(EXAMPLE_NAME ${PROJECT_NAME}_basic_test)

add_executable(${EXAMPLE_NAME}
	test_tinymat.cpp
)
target_link_libraries(${EXAMPLE_NAME} TinyMAT::TinyMAT)

# Installation
install(TARGETS ${EXAMPLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/basic_test_read.m DESTINATION ${CMAKE_INSTALL_BINDIR})
