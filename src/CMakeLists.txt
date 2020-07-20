cmake_minimum_required(VERSION 3.10)

set(lib_name "libTinyMAT")
set(libsh_name "libTinyMATShared")

# Set up source files
set(SOURCES
    tinymatwriter.cpp
)
set(HEADERS
    tinymatwriter.h
)


include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(CheckSymbolExists)
check_symbol_exists(fopen_s "stdio.h" HAVE_FOPEN_S)
check_symbol_exists(sprintf_s "stdio.h" HAVE_SPRINTF_S)
check_symbol_exists(memcpy_s "string.h" HAVE_MEMCPY_S)
check_symbol_exists(gmtime_s "time.h" HAVE_GMTIME_S)


if(TinyMAT_BUILD_SHARED_LIBS)
    add_library(${libsh_name} SHARED  ${SOURCES} ${HEADERS})
    target_compile_features(${libsh_name} PUBLIC cxx_std_11)
    set_property(TARGET ${libsh_name} PROPERTY VERSION "${PROJECT_VERSION}")
    set_property(TARGET ${libsh_name} PROPERTY OUTPUT_NAME  "${libsh_name}${TinyMAT_LIBNAME_ADDITION}")
    target_include_directories(${libsh_name} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  
    )
    set_target_properties(${libsh_name} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS "ON")
    if (HAVE_FOPEN_S)
        target_compile_definitions(${libsh_name} PRIVATE HAVE_FOPEN_S)
    endif()
    if (HAVE_SPRINTF_S)
        target_compile_definitions(${libsh_name} PRIVATE HAVE_SPRINTF_S)
    endif()
    if (HAVE_MEMCPY_S)
        target_compile_definitions(${libsh_name} PRIVATE HAVE_MEMCPY_S)
    endif()
    if (HAVE_GMTIME_S)
        target_compile_definitions(${libsh_name} PRIVATE HAVE_GMTIME_S)
    endif()
    if(TinyMAT_FILEBACKEND_USE_MEMORY_CACHE)
        target_compile_definitions(${libsh_name} PRIVATE TINYMAT_WRITE_VIA_MEMORY)
    endif()
    if(TinyMAT_OPENCV_SUPPORT)
        target_compile_definitions(${libsh_name} PUBLIC TINYMAT_USES_OPENCV)
        target_include_directories(${libsh_name} PUBLIC ${OpenCV_INCLUDE_DIRS})
        target_link_libraries(${libsh_name} PUBLIC ${OpenCV_LIBS})
        target_compile_definitions(${libsh_name} PUBLIC TINYMAT_USES_OPENCV)
    endif()
    if(TinyMAT_QT5_SUPPORT)
        target_compile_definitions(${libsh_name} PUBLIC TINYMAT_USES_QVARIANT)
        target_link_libraries(${libsh_name} PUBLIC Qt5::Core)
    endif()

    generate_export_header(${libsh_name} BASE_NAME tinymatwriter)
    target_include_directories(${libsh_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_include_directories(${libsh_name} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_compile_definitions(${libsh_name} PRIVATE ${libsh_name}_EXPORTS)
endif()

if(TinyMAT_BUILD_STATIC_LIBS)
    add_library(${lib_name} STATIC  ${SOURCES} ${HEADERS})
    target_compile_features(${lib_name} PUBLIC cxx_std_11)
    set_property(TARGET ${lib_name} PROPERTY VERSION "${PROJECT_VERSION}")
    set_property(TARGET ${lib_name} PROPERTY OUTPUT_NAME  "${lib_name}${TinyMAT_LIBNAME_ADDITION}")
    target_include_directories(${lib_name} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  
        )
    if (HAVE_FOPEN_S)
        target_compile_definitions(${lib_name} PRIVATE HAVE_FOPEN_S)
    endif()
    if (HAVE_SPRINTF_S)
        target_compile_definitions(${lib_name} PRIVATE HAVE_SPRINTF_S)
    endif()
    if (HAVE_MEMCPY_S)
        target_compile_definitions(${lib_name} PRIVATE HAVE_MEMCPY_S)
    endif()
    if (HAVE_GMTIME_S)
        target_compile_definitions(${lib_name} PRIVATE HAVE_GMTIME_S)
    endif()
    if(TinyMAT_FILEBACKEND_USE_MEMORY_CACHE)
        target_compile_definitions(${lib_name} PRIVATE TINYMAT_WRITE_VIA_MEMORY)
    endif()
    if(TinyMAT_OPENCV_SUPPORT)
        target_compile_definitions(${lib_name} PUBLIC TINYMAT_USES_OPENCV)
        target_include_directories(${lib_name} PUBLIC ${OpenCV_INCLUDE_DIRS})
        target_link_libraries(${lib_name} PUBLIC ${OpenCV_LIBS})
    endif()
    if(TinyMAT_QT5_SUPPORT)
        target_compile_definitions(${lib_name} PUBLIC TINYMAT_USES_QVARIANT)
        target_link_libraries(${lib_name} PUBLIC Qt5::Core)
    endif()


    if(NOT TinyMAT_BUILD_SHARED_LIBS)
      generate_export_header(${lib_name} BASE_NAME tinymatwriter)
    endif()
    target_include_directories(${lib_name} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_include_directories(${lib_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_include_directories(${lib_name} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_compile_definitions(${lib_name} PUBLIC TINYMATWRITER_STATIC_DEFINE)
endif()



# Installation
include(GNUInstallDirs)
if(TinyMAT_BUILD_SHARED_LIBS)
        write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${libsh_name}Version.cmake
                                     VERSION ${PROJECT_VERSION}
                                     COMPATIBILITY AnyNewerVersion )
        install(TARGETS ${libsh_name} EXPORT ${libsh_name}_TARGETS
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)
	install(EXPORT ${libsh_name}_TARGETS
		FILE ${libsh_name}Config.cmake 
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${libsh_name}
	)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${libsh_name}Version.cmake" 
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${libsh_name}  )
endif(TinyMAT_BUILD_SHARED_LIBS)

if(TinyMAT_BUILD_STATIC_LIBS)
        write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Version.cmake
                                     VERSION ${PROJECT_VERSION}
                                     COMPATIBILITY AnyNewerVersion )
        install(TARGETS ${lib_name} EXPORT ${lib_name}_TARGETS
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)
	install(EXPORT ${lib_name}_TARGETS
		FILE ${lib_name}Config.cmake 
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${lib_name}
	)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Version.cmake" 
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${lib_name}  )
endif(TinyMAT_BUILD_STATIC_LIBS)

install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Headers)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tinymatwriter_export.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )
configure_file(${CMAKE_SOURCE_DIR}/readme.txt.in ${CMAKE_CURRENT_BINARY_DIR}/readme.txt @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/readme.txt" DESTINATION "${TinyMAT_DOC_INSTALL_DIR}/${lib_name}/" )
install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION "${TinyMAT_DOC_INSTALL_DIR}/${lib_name}/" )
install(FILES "${CMAKE_SOURCE_DIR}/README.md" DESTINATION "${TinyMAT_DOC_INSTALL_DIR}/${lib_name}/" )


