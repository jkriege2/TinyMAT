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
    if(TinyMAT_FILEBACKEND_USE_MEMORY_CACHE)
        target_compile_definitions(${libsh_name} PRIVATE TINYMAT_WRITE_VIA_MEMORY)
    endif()
    if(TinyMAT_OPENCV_SUPPORT)
        target_compile_definitions(${libsh_name} PUBLIC TINYMAT_USES_OPENCV)
		target_include_directories(${libsh_name} PUBLIC ${OpenCV_INCLUDE_DIRS})
		target_link_libraries(${libsh_name} PUBLIC ${OpenCV_LIBS})
    endif()
    if(TinyMAT_OPENCV_SUPPORT)
        target_compile_definitions(${libsh_name} PUBLIC TINYMAT_USES_OPENCV)
    endif()
    if(TinyMAT_QT5_SUPPORT)
        target_compile_definitions(${libsh_name} PUBLIC TINYMAT_USES_QVARIANT)
        target_link_libraries(${libsh_name} PUBLIC Qt5::Core)
    endif()
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

install(FILES ${HEADERS}
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
		COMPONENT Headers)

configure_file(${CMAKE_SOURCE_DIR}/readme.txt.in ${CMAKE_CURRENT_BINARY_DIR}/readme.txt @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/readme.txt" DESTINATION "${TinyMAT_DOC_INSTALL_DIR}/${lib_name}/" )
install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION "${TinyMAT_DOC_INSTALL_DIR}/${lib_name}/" )
install(FILES "${CMAKE_SOURCE_DIR}/README.md" DESTINATION "${TinyMAT_DOC_INSTALL_DIR}/${lib_name}/" )


