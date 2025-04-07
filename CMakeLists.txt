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

	message(STATUS "X11_FOUND = ${X11_FOUND}")
	message(STATUS "WAYLAND_CLIENT_FOUND = ${WAYLAND_CLIENT_FOUND}")

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
file(GLOB_RECURSE WINDOW_SRC_FILES CONFIGURE_DEPENDS
    ${WINDOW_SRC_DIR}/*.cpp
	${WINDOW_SRC_DIR}/*/*.cpp
)

# Build as a shared library (DLL)
target_sources(KalaWindow PRIVATE ${WINDOW_SRC_FILES})
set_target_properties(KalaWindow PROPERTIES OUTPUT_NAME "KalaWindow")
target_compile_features(KalaWindow PRIVATE cxx_std_20)

# Append 'D' suffix to debug builds for both Windows and Linux
set_target_properties(KalaWindow PROPERTIES
    OUTPUT_NAME "KalaWindow$<$<CONFIG:Debug>:D>"
)

# Apply all directories to target
target_include_directories(KalaWindow PRIVATE
	# KalaWindow headers
    ${WINDOW_INCLUDE_DIR}
	
	# External headers
	${EXT_GLM_DIR}
	${EXT_MAGIC_ENUM_DIR}
	${EXT_CRASH_DIR}

	${WAYLAND_CLIENT_INCLUDE_DIRS}
)

if (KALAKIT_WINDOWS)
	# Define as Windows
	target_compile_definitions(KalaWindow PRIVATE KALAKIT_WINDOWS=1)
	
    target_link_libraries(KalaWindow PRIVATE
		${CRASH_LIBRARY_PATH}
		opengl32)
else()
	if (KALAKIT_WAYLAND)
		# Define as Wayland
		target_compile_definitions(KalaWindow PRIVATE KALAKIT_WAYLAND=1)
		
		target_link_libraries(KalaWindow PRIVATE
			${CRASH_LIBRARY_PATH}
			${WAYLAND_CLIENT_LIBRARIES})
	else()
		# Define as X11
		target_compile_definitions(KalaWindow PRIVATE KALAKIT_X11=1)
		
		target_link_libraries(KalaWindow PRIVATE
			${CRASH_LIBRARY_PATH}
			${X11_LIBRARIES})
	endif()
endif()

# Define preprocessor directives
if(KALAKIT_DEBUG)
	target_compile_definitions(KalaWindow PRIVATE 
		KALAWINDOW_DLL_EXPORT
		KALAWINDOW_DEBUG=1
	)
else()
	target_compile_definitions(KalaWindow PRIVATE 
		KALAWINDOW_DLL_EXPORT
		KALAWINDOW_DEBUG=0
	)
endif()

# Copy all DLLs to the install directory after build
if(KALAKIT_RELEASE)
	set(DLL_ORIGIN_DIR "${CMAKE_SOURCE_DIR}/files/external dlls/release")
else()
	set(DLL_ORIGIN_DIR "${CMAKE_SOURCE_DIR}/files/external dlls/debug")
endif()
set(DLL_TARGET_DIR "$<TARGET_FILE_DIR:KalaWindow>")

file(GLOB DLL_FILES "${DLL_ORIGIN_DIR}/*")
	
foreach(DLL_FILE ${DLL_FILES})
    get_filename_component(DLL_NAME ${DLL_FILE} NAME)
    add_custom_command(TARGET KalaWindow POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
            ${DLL_FILE} 
            ${DLL_TARGET_DIR}/${DLL_NAME}
    )
endforeach()

# Install the DLL and its import library (KalaWindow.dll + KalaWindow.lib)
install(TARGETS KalaWindow
    RUNTIME DESTINATION bin  # .dll (Windows) and .so (Linux)
    LIBRARY DESTINATION lib  # .lib (Windows import lib) or .so (Linux)
    ARCHIVE DESTINATION lib  # .lib (Linux static lib)
)

# Install header files
install(DIRECTORY ${WINDOW_INCLUDE_DIR}/
    DESTINATION include
    FILES_MATCHING 
		PATTERN "*.hpp" 
		PATTERN "*.h" 
		PATTERN "*.inl"
)
