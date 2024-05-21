project(extcmake_tinymat_test CXX)
cmake_minimum_required(VERSION 3.0)

set(EXAMPLE_NAME extcmake_tinymat_test)

# now find libTinyMAT
find_package(TinyMAT REQUIRED)

# add an executable
add_executable(${EXAMPLE_NAME} extcmake_tinymat_test.cpp)

# link against libTinyMAT
target_link_libraries(${EXAMPLE_NAME} TinyMAT::TinyMAT)
