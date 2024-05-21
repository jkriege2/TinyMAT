cmake_minimum_required(VERSION 3.10)

######################################################################################################
# Project Name and Version
project(TinyMAT LANGUAGES CXX VERSION 4.0.0.0)

# set search path for CMake files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)


######################################################################################################
# ensure a build-type is set (Release is default)
include(tinymat_build_type)

######################################################################################################
# compile options:
if(NOT DEFINED TinyMAT_FILEBACKEND_USE_MEMORY_CACHE)
    option(TinyMAT_FILEBACKEND_USE_MEMORY_CACHE "Build with a file-backend, that caches the output in memory, before writing to disk (sets TINYMAT_WRITE_VIA_MEMORY when building)" ON)
endif()
if(NOT DEFINED BUILD_SHARED_LIBS)
    option(BUILD_SHARED_LIBS "Build as shared library" ON)
endif()
if(NOT DEFINED TinyMAT_OPENCV_SUPPORT)
    find_package(OpenCV)
    option(TinyMAT_OPENCV_SUPPORT "Build with Support for OpenCV" ${OpenCV_FOUND})
endif()
if(NOT DEFINED TinyMAT_QT_SUPPORT)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
    if(${Qt5_FOUND})
        set(TinyMAT_QT_SUPPORT ON)
    else()
        if(${Qt6_FOUND})
            set(TinyMAT_QT_SUPPORT ON)
        endif()
    endif()
    option(TinyMAT_QT_SUPPORT "Build with Support for Qt5/6" ${TinyMAT_QT_SUPPORT})
endif()
if(NOT DEFINED TinyMAT_BUILD_DECORATE_LIBNAMES_WITH_BUILDTYPE)
    option(TinyMAT_BUILD_DECORATE_LIBNAMES_WITH_BUILDTYPE "If set, the build-type (debug/release/...) is appended to the library name" ON)
endif()
if(NOT DEFINED TinyMAT_BUILD_EXAMPLES)
    option(TinyMAT_BUILD_EXAMPLES "Build the examples examples" ON)
endif()
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    option(CMAKE_INSTALL_PREFIX "Install directory" ${CMAKE_CURRENT_SOURCE_DIR}/install)
endif()
if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # We're in the root, define additional targets for developers.
    # This prepares the library to be used with CMake's FetchContent
    set(TinyMAT_BUILD_EXAMPLES OFF)
endif()


######################################################################################################
#evaluate the options above
if (NOT CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")
endif()

if (NOT CMAKE_INSTALL_BINDIR)
    set(CMAKE_INSTALL_BINDIR "bin")
endif()

if (NOT CMAKE_INSTALL_INCLUDEDIR)
    set(CMAKE_INSTALL_INCLUDEDIR "include")
endif()

if (NOT TinyMAT_DOC_INSTALL_DIR)
    set(TinyMAT_DOC_INSTALL_DIR "share/doc/")
endif()


# place all DLLs and EXEs in the subdirectory output of the top level directory of the build tree
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)


if(TinyMAT_BUILD_DECORATE_LIBNAMES_WITH_BUILDTYPE)
    set(TinyMAT_LIBNAME_ADDITION "_$<CONFIG>")
else()
    set(TinyMAT_LIBNAME_ADDITION )
endif()


######################################################################################################
# check for dependency libs
if (TinyMAT_QT_SUPPORT)
    # check for Qt

    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
    if(QT_VERSION_MAJOR LESS 5)
        message(FATAL_ERROR "Minimum supported Qt version is 5, but you are trying to compile against Qt${QT_VERSION_MAJOR}")
    endif()

    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
    if (${Qt5_FOUND})
        message(NOTICE "compiling ${PROJECT_NAME} with Qt5-support")
    else()
        if (${Qt6_FOUND})
            message(NOTICE "compiling ${PROJECT_NAME} with Qt6-support")
        else()
            message(FATAL_ERROR "could not find Qt5 on your system")
        endif()
    endif()
endif()
if (TinyMAT_OPENCV_SUPPORT)
    # check for Qt
    find_package(OpenCV REQUIRED)
    if (${OpenCV_FOUND})
        message(NOTICE "compiling ${PROJECT_NAME} with OpenCV-support")
    else()
        message(FATAL_ERROR "could not find OpenCV on your system")
    endif()
endif()


######################################################################################################
# now add subdirectories with the library code ...
add_subdirectory(src)

# ... and optionally the examples
if(TinyMAT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()



