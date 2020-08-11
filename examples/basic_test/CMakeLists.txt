cmake_minimum_required(VERSION 3.0)

set(EXAMPLE_NAME ${PROJECT_NAME}_basic_test)

add_executable(${EXAMPLE_NAME}
	test_tinymat.cpp
)
if(TinyMAT_BUILD_STATIC_LIBS)
    target_link_libraries(${EXAMPLE_NAME} TinyMAT)
elseif(TinyMAT_BUILD_SHARED_LIBS)
    target_link_libraries(${EXAMPLE_NAME} TinyMATShared)
endif()

# Installation
install(TARGETS ${EXAMPLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/basic_test_read.m DESTINATION ${CMAKE_INSTALL_BINDIR})
