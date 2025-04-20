cmake_minimum_required(VERSION 3.30.3)

# Ensure install path is set correctly
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/out/install")
endif()

# Specify the compiler for Linux builds
if (UNIX)
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 0)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("KalaWindow" VERSION ${PROJECT_VERSION})

add_library(KalaWindow SHARED)

# Set OS global type
if (WIN32)
    set(KALAKIT_WINDOWS TRUE)
    target_compile_definitions(KalaWindow PRIVATE KALAKIT_WINDOWS)
elseif(UNIX)
    find_package(X11 QUIET)
    find_package(OpenGL REQUIRED)

    include(FindPkgConfig)
	pkg_check_modules(EGL QUIET egl)

	message(STATUS "[KALAKIT] X11_FOUND = ${X11_FOUND}")
	message(STATUS "[KALAKIT] EGL_FOUND = ${EGL_FOUND}")
	message(STATUS "[KALAKIT] OpenGL_FOUND: ${OpenGL_FOUND}")

	if (X11_FOUND)
		set(KALAKIT_X11 TRUE)
		message(STATUS "[KALAKIT] Using X11 backend")
		target_compile_definitions(KalaWindow PRIVATE KALAKIT_X11)
	else()
		message(FATAL_ERROR "[KALAKIT] No valid backend found! Must support X11.")
	endif()
else()
	message(FATAL_ERROR "[KALAKIT] Unsupported platform version! Must be WIN32 or UNIX!")
endif()

# Set build global type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(KALAKIT_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	set(KALAKIT_RELEASE TRUE)
else()
	message(FATAL_ERROR "[KALAKIT] Unknown CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'! Must be Debug, Release, or RelWithDebInfo.")
endif()

# Set runtime library type for MSVC
if (MSVC)
    if(KALAKIT_DEBUG)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
endif()

# Paths to frequently used directories
get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")

# Window files
set(WINDOW_SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(WINDOW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# External files
set(EXT_GLM_DIR "${CMAKE_SOURCE_DIR}/include/glm")
set(EXT_MAGIC_ENUM_DIR "${CMAKE_SOURCE_DIR}/include/magic_enum")
set(EXT_CRASH_DIR "${CMAKE_SOURCE_DIR}/_external_shared/KalaCrashHandler")

# Add KalaCrashHandler
if (KALAKIT_WINDOWS)
	if(KALAKIT_RELEASE)
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/release/KalaCrashHandler.lib")
	else()
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/debug/KalaCrashHandlerD.lib")
	endif()
else()
	if(KALAKIT_RELEASE)
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/release/libKalaCrashHandler.so")
	else()
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/debug/libKalaCrashHandlerD.so")
	endif()
endif()

# Add source files
set(WINDOW_SRC_FILES)

# Root level source files
file(GLOB ROOT_SRC CONFIGURE_DEPENDS
    ${WINDOW_SRC_DIR}/*.cpp
)
list(APPEND WINDOW_SRC_FILES ${ROOT_SRC})

# Windows source files
if (KALAKIT_WINDOWS)
    file(GLOB WINDOWS_SRC
        ${WINDOW_SRC_DIR}/windows/*.cpp
    )
    list(APPEND WINDOW_SRC_FILES ${WINDOWS_SRC})

# X11 source files
elseif (KALAKIT_X11)
    file(GLOB X11_SRC
        ${WINDOW_SRC_DIR}/x11/*.cpp
    )
    list(APPEND WINDOW_SRC_FILES ${X11_SRC})
endif()

# Shared library
target_sources(KalaWindow PRIVATE ${WINDOW_SRC_FILES})
set_target_properties(KalaWindow PROPERTIES OUTPUT_NAME "KalaWindow$<$<CONFIG:Debug>:D>")
target_compile_features(KalaWindow PRIVATE cxx_std_20)

# Static library
if (NOT KALAKIT_WINDOWS)
	add_library(KalaWindowStatic STATIC  ${WINDOW_SRC_FILES})
	set_target_properties(KalaWindowStatic PROPERTIES OUTPUT_NAME "KalaWindow$<$<CONFIG:Debug>:D>")
	target_compile_features(KalaWindowStatic PRIVATE cxx_std_20)
endif()

# Apply all directories
function(set_kalawindow_includes target)
	target_include_directories(${target} PRIVATE
		# KalaWindow headers
		"${WINDOW_INCLUDE_DIR}"

		# External headers
		"${EXT_GLM_DIR}"
		"${EXT_MAGIC_ENUM_DIR}"
		"${EXT_CRASH_DIR}"
		)
	if(KALAKIT_X11)
		target_include_directories(${target} PRIVATE
			"${EGL_INCLUDE_DIRS}"
		)
	endif()
endfunction()

set_kalawindow_includes(KalaWindow)
if (NOT KALAKIT_WINDOWS)
	set_kalawindow_includes(KalaWindowStatic)
endif()

function(configure_kalawindow_target target)
	if (KALAKIT_WINDOWS)
		target_compile_definitions(${target} PRIVATE KALAKIT_WINDOWS=1)

		target_link_libraries(${target} PRIVATE
			opengl32
			${CRASH_LIBRARY_PATH}
		)
	else()
		target_compile_definitions(${target} PRIVATE KALAKIT_X11=1)

		target_link_libraries(${target} PRIVATE
			OpenGL::GL
			${CRASH_LIBRARY_PATH}
			${X11_LIBRARIES}
		)
	endif()
endfunction()

configure_kalawindow_target(KalaWindow)
if (NOT KALAKIT_WINDOWS)
	configure_kalawindow_target(KalaWindowStatic)
endif()

# Define preprocessor directives
function(set_debug_defines target)
	if(KALAKIT_DEBUG)
		target_compile_definitions(${target} PRIVATE
			KALAWINDOW_DLL_EXPORT
			KALAWINDOW_DEBUG=1
		)
	else()
		target_compile_definitions(${target} PRIVATE
			KALAWINDOW_DLL_EXPORT
			KALAWINDOW_DEBUG=0
		)
	endif()
endfunction()

set_debug_defines(KalaWindow)
if (NOT KALAKIT_WINDOWS)
	set_debug_defines(KalaWindowStatic)
endif()

# Copy shared library to bin (.dll or .so)
add_custom_command(TARGET KalaWindow POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin"

    # Copy the shared library to the bin directory
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:KalaWindow>
        "${CMAKE_BINARY_DIR}/bin"

    # Remove the original
    COMMAND ${CMAKE_COMMAND} -E remove
        $<TARGET_FILE:KalaWindow>
)

if (NOT KALAKIT_WINDOWS)
	# Copy static library to lib (.lib or .a)
	add_custom_command(TARGET KalaWindowStatic POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/lib"

		# Copy the static library to the lib directory
		COMMAND ${CMAKE_COMMAND} -E copy
			$<TARGET_LINKER_FILE:KalaWindowStatic>
			"${CMAKE_BINARY_DIR}/lib"

		# Remove the original
		COMMAND ${CMAKE_COMMAND} -E remove
			$<TARGET_LINKER_FILE:KalaWindowStatic>
	)
endif()

# Copy all project-related headers to include folder
file(GLOB PUBLIC_HEADERS "${WINDOW_INCLUDE_DIR}/*.hpp")
foreach (header ${PUBLIC_HEADERS})
	get_filename_component(fname "${header}" NAME)
	add_custom_command(TARGET KalaWindow POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			"${header}"
			"${CMAKE_BINARY_DIR}/include/${fname}"
	)
endforeach()
