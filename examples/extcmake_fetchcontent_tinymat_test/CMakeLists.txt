# set minimum required CMake-Version
cmake_minimum_required(VERSION 3.23)

# set Project name
project(extcmake_fetchcontent_tinymat_test LANGUAGES CXX)

# include TinyMAT via FetchContent-API:
# ... first load the FetchContent-API:
include(FetchContent) # once in the project to include the module
# ... now declare TinyMAT
FetchContent_Declare(TinyMAT
                     GIT_REPOSITORY https://github.com/jkriege2/TinyMAT.git
                     # GIT_TAG        3.1.0.0
                     )
                     
# ... finally make TinyMAT available
FetchContent_MakeAvailable(TinyMAT)

add_executable(${PROJECT_NAME}
	extcmake_fetchcontent_tinymat_test.cpp
)

target_link_libraries(${PROJECT_NAME} TinyMAT::TinyMAT)
