project(extcmake_tinymat_test CXX)
cmake_minimum_required(VERSION 3.0)

set(EXAMPLE_NAME extcmake_tinymat_test)

# if you built with CMake and Qt5 support, you also need to include these packages
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(OpenCV)

# now find libTinyMAT
find_package(libTinyMAT REQUIRED)

# add an executable
add_executable(${EXAMPLE_NAME} extcmake_tinymat_test.cpp)

# link against libTinyMAT
target_link_libraries(${EXAMPLE_NAME} libTinyMAT)
