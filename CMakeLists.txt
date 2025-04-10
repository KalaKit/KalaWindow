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

    include(FindPkgConfig)
	pkg_check_modules(WAYLAND_CLIENT QUIET wayland-client)
	pkg_check_modules(WAYLAND_EGL QUIET wayland-egl)
	pkg_check_modules(EGL QUIET egl)

	message(STATUS "X11_FOUND = ${X11_FOUND}")
	message(STATUS "WAYLAND_CLIENT_FOUND = ${WAYLAND_CLIENT_FOUND}")
	message(STATUS "WAYLAND_EGL_FOUND = ${WAYLAND_EGL_FOUND}")
	message(STATUS "EGL_FOUND = ${EGL_FOUND}")

	find_package(OpenGL REQUIRED)
	message(STATUS "OpenGL_FOUND: ${OpenGL_FOUND}")

    if (WAYLAND_CLIENT_FOUND)
		set(KALAKIT_WAYLAND TRUE)
		message(STATUS "[KALAKIT] Using Wayland backend")
		target_compile_definitions(KalaWindow PRIVATE KALAKIT_WAYLAND)
	elseif (X11_FOUND)
		set(KALAKIT_X11 TRUE)
		message(STATUS "[KALAKIT] Using X11 backend")
		target_compile_definitions(KalaWindow PRIVATE KALAKIT_X11)
	else()
		message(FATAL_ERROR "[KALAKIT] No backend found! Must support Wayland or X11.")
	endif()
else()
	message(FATAL_ERROR "Unsupported OS version! Must be WIN32 or UNIX!")
endif()

# Set build global type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(KALAKIT_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	set(KALAKIT_RELEASE TRUE)
else()
	message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'! Must be Debug, Release, or RelWithDebInfo.")
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
set(EXT_XDG_DIR "${CMAKE_SOURCE_DIR}/_external_shared/xdg-shell")

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

# Wayland source files
elseif (KALAKIT_WAYLAND)
    file(GLOB WAYLAND_SRC
		"${EXT_XDG_DIR}/xdg-shell-protocol.c"
        "${WINDOW_SRC_DIR}/wayland/*.cpp"
    )
    list(APPEND WINDOW_SRC_FILES ${WAYLAND_SRC})

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
add_library(KalaWindowStatic STATIC  ${WINDOW_SRC_FILES})
set_target_properties(KalaWindowStatic PROPERTIES OUTPUT_NAME "KalaWindow$<$<CONFIG:Debug>:D>")
target_compile_features(KalaWindowStatic PRIVATE cxx_std_20)

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
	if (KALAKIT_WAYLAND)
		target_include_directories(${target} PRIVATE
			"${EXT_XDG_DIR}"
			"${WAYLAND_CLIENT_INCLUDE_DIRS}"
			"${WAYLAND_EGL_INCLUDE_DIRS}"
			"${EGL_INCLUDE_DIRS}"
		)
	elseif(KALAKIT_X11)
		target_include_directories(${target} PRIVATE
			"${EGL_INCLUDE_DIRS}"
		)
	endif()
endfunction()

set_kalawindow_includes(KalaWindow)
set_kalawindow_includes(KalaWindowStatic)

function(configure_kalawindow_target target)
	if (KALAKIT_WINDOWS)
		target_compile_definitions(${target} PRIVATE KALAKIT_WINDOWS=1)

		target_link_libraries(${target} PRIVATE
			opengl32
			${CRASH_LIBRARY_PATH}
		)
	else()
		if (KALAKIT_WAYLAND)
			target_compile_definitions(${target} PRIVATE KALAKIT_WAYLAND=1)

			target_link_libraries(${target} PRIVATE
				OpenGL::GL
				EGL
				${CRASH_LIBRARY_PATH}
				${WAYLAND_CLIENT_LIBRARIES}
				${WAYLAND_EGL_LIBRARIES}
				${EGL_LIBRARIES}
			)
		else()
			target_compile_definitions(${target} PRIVATE KALAKIT_X11=1)

			target_link_libraries(${target} PRIVATE
				OpenGL::GL
				EGL
				${CRASH_LIBRARY_PATH}
				${X11_LIBRARIES}
			)
		endif()
	endif()
endfunction()

configure_kalawindow_target(KalaWindow)
configure_kalawindow_target(KalaWindowStatic)

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
set_debug_defines(KalaWindowStatic)

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

# Copy all headers to include folder
add_custom_command(TARGET KalaWindow POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/include"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/include/KalaCrashHandler"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/include/glm"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/include/magic_enum"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${WINDOW_INCLUDE_DIR}/"
        "${CMAKE_BINARY_DIR}/include"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${EXT_CRASH_DIR}/"
        "${CMAKE_BINARY_DIR}/include/KalaCrashHandler"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${EXT_MAGIC_ENUM_DIR}/"
        "${CMAKE_BINARY_DIR}/include/magic_enum"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${EXT_GLM_DIR}/"
        "${CMAKE_BINARY_DIR}/include/glm"
)
