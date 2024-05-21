cmake_minimum_required(VERSION 3.10)

set(lib_name "TinyMAT")


include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(CheckSymbolExists)


message(STATUS "Resolving GIT Version")

set(_build_version "unknown")

find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE TINYMAT_GITVERSION
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message( STATUS "GIT hash: ${TINYMAT_GITVERSION} (from ${PROJECT_SOURCE_DIR})")
else()
  message(STATUS "GIT not found")
endif()


string(TIMESTAMP TINYMAT_COMPILETIME "%Y-%m-%d %H:%M:%S")
configure_file(${CMAKE_CURRENT_LIST_DIR}/tinymat_version.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/tinymat_version.h)


check_symbol_exists(strcpy_s "string.h" HAVE_STRCPY_S)
check_symbol_exists(fopen_s "stdio.h" HAVE_FOPEN_S)
check_symbol_exists(fread_s "stdio.h" HAVE_FREAD_S)
check_symbol_exists(sprintf_s "stdio.h" HAVE_SPRINTF_S)
check_symbol_exists(strcat_s "string.h" HAVE_STRCAT_S)
check_symbol_exists(memcpy_s "string.h" HAVE_MEMCPY_S)
check_symbol_exists(memset_s "string.h" HAVE_MEMSET_S)
check_symbol_exists(strnlen_s "string.h" HAVE_STRNLEN_S)
check_symbol_exists(_ftelli64 "stdio.h" HAVE_FTELLI64)
check_symbol_exists(_fseeki64 "stdio.h" HAVE_FSEEKI64)
check_symbol_exists(ftello64 "stdio.h" HAVE_FTELLO64)
check_symbol_exists(fseeko64 "stdio.h" HAVE_FSEEKO64)
check_symbol_exists(gmtime_s "time.h" HAVE_GMTIME_S)





# add libraries for C and C++ separately
add_library(${lib_name} )
set_property(TARGET ${lib_name} PROPERTY VERSION "${PROJECT_VERSION}")
set_property(TARGET ${lib_name} PROPERTY OUTPUT_NAME "${lib_name}${TinyMAT_LIBNAME_ADDITION}")
target_include_directories(${lib_name}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(${lib_name}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
if (BUILD_SHARED_LIBS)
    set_target_properties(${lib_name} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS "ON")
else()
    target_compile_definitions(${lib_name} PUBLIC TINYMAT_STATIC_DEFINE)
endif(BUILD_SHARED_LIBS)



if(TinyMAT_FILEBACKEND_USE_MEMORY_CACHE)
    target_compile_definitions(${lib_name} PRIVATE TINYMAT_WRITE_VIA_MEMORY)
endif()
if(TinyMAT_OPENCV_SUPPORT)
    target_compile_definitions(${lib_name} PUBLIC TINYMAT_USES_OPENCV)
    target_include_directories(${lib_name} PUBLIC ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(${lib_name} PUBLIC ${OpenCV_LIBS})
endif()
if(TinyMAT_QT_SUPPORT)
    target_compile_definitions(${lib_name} PUBLIC TINYMAT_USES_QVARIANT)
    target_link_libraries(${lib_name} PUBLIC Qt${QT_VERSION_MAJOR}::Core)
endif()

if (HAVE_STRCPY_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_STRCPY_S)
endif()
if (HAVE_FOPEN_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_FOPEN_S)
endif()
if (HAVE_SPRINTF_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_SPRINTF_S)
endif()
if (HAVE_STRCAT_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_STRCAT_S)
endif()
if (HAVE_FREAD_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_FREAD_S)
endif()
if (HAVE_MEMCPY_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_MEMCPY_S)
endif()
if (HAVE_MEMSET_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_MEMSET_S)
endif()
if (HAVE_STRNLEN_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_STRNLEN_S)
endif()
if (HAVE_FTELLI64)
    target_compile_definitions(${lib_name} PRIVATE HAVE_FTELLI64)
endif()
if (HAVE_FSEEKI64)
    target_compile_definitions(${lib_name} PRIVATE HAVE_FSEEKI64)
endif()
if (HAVE_FTELLO64)
    target_compile_definitions(${lib_name} PRIVATE HAVE_FTELLO64)
endif()
if (HAVE_FSEEKO64)
    target_compile_definitions(${lib_name} PRIVATE HAVE_FSEEKO64)
endif()
if (HAVE_GMTIME_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_GMTIME_S)
endif()


# ... add an alias with the correct namespace
add_library(TinyMAT::${lib_name} ALIAS ${lib_name})





# Set up source files
target_sources(${lib_name} PRIVATE
    tinymatwriter.cpp
)


target_sources(${lib_name} PUBLIC FILE_SET HEADERS TYPE HEADERS
    FILES
        tinymatwriter.h
)


generate_export_header(${lib_name} BASE_NAME tinymat)




# Installation
include(GNUInstallDirs)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Version.cmake
                             VERSION ${PROJECT_VERSION}
                             COMPATIBILITY AnyNewerVersion )

install(TARGETS ${lib_name}
        EXPORT ${lib_name}_TARGETS
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
install(EXPORT ${lib_name}_TARGETS
        FILE ${lib_name}Config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${lib_name}
        NAMESPACE TinyMAT::
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${lib_name}  )


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tinymat_export.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tinymat_version.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )
configure_file(${PROJECT_SOURCE_DIR}/readme.txt.in ${CMAKE_CURRENT_BINARY_DIR}/readme.txt @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/readme.txt" DESTINATION "${TinyMAT_DOC_INSTALL_DIR}/${lib_name}/" )
install(FILES "${PROJECT_SOURCE_DIR}/LICENSE" DESTINATION "${TinyMAT_DOC_INSTALL_DIR}/${lib_name}/" )
install(FILES "${PROJECT_SOURCE_DIR}/README.md" DESTINATION "${TinyMAT_DOC_INSTALL_DIR}/${lib_name}/" )


























