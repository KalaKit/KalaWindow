cmake_minimum_required(VERSION 3.29.2)

set(PROGRAM_VERSION "KalaWindow 1.0")
set(PROGRAM_VERSION_NUMBER 1.0.0.0)

project("KalaWindow" VERSION ${PROGRAM_VERSION_NUMBER} LANGUAGES C CXX)

# Adds a '_' prefix in front of the created binary files if set to TRUE
set(KALA_PRERELEASE TRUE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Runtime lib handling for MSVC
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Platform Detection
if (WIN32)
    message(STATUS "[KALAWINDOW] Platform = Windows")
else()
    message(FATAL_ERROR "[KALAWINDOW] Unsupported platform. Only Windows is supported.")
endif()

# Build Type Detection
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IS_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(IS_RELEASE TRUE)
else()
    message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'! Must be Debug, Release, or RelWithDebInfo.")
endif()

# Dependencies
find_package(OpenGL REQUIRED)
# find_package(Vulkan REQUIRED)

# Paths
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

set(EXT_SHARED_DIR "${CMAKE_SOURCE_DIR}/_external_shared")

# Source Files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*/*.cpp"
)

# SHARED LIBRARY (.dll)
add_library(KalaWindow SHARED ${SOURCE_FILES})

# Enable C++ exception handling semantics for MSVC
if (MSVC)
    target_compile_options(KalaWindow PRIVATE /EHsc)
endif()
		
# Get suffix
if (IS_RELEASE)
	set(KALA_SUFFIX "")
else()
	set(KALA_SUFFIX "d")
endif()

# Set output name with renderer suffix and debug postfix
set_target_properties(KalaWindow PROPERTIES
	OUTPUT_NAME "KalaWindow${PROJECT_VERSION_MAJOR}${KALA_SUFFIX}"
)

file(GLOB_RECURSE HEADERS configure_depends
	"${CMAKE_SOURCE_DIR}/include/*.hpp"
)
target_sources(KalaWindow PRIVATE ${HEADERS})
target_include_directories(KalaWindow PRIVATE 
	${INCLUDE_DIR}
	${EXT_SHARED_DIR}
)

target_compile_definitions(KalaWindow PRIVATE
	ENABLE_EMIT_LOG     # allow user to define EmitLog in log_utils.hpp
	LIB_EXPORT          # dll import/export
	WIN32_LEAN_AND_MEAN # cleaner windows.h
	NOMINMAX            # skips min/max windows macros in favor of the std variants
	UNICODE             # selects wide api variants over ansi for UTF16 windows functions
	_UNICODE            # uses wide text types in the C runtime layer
)

# Link libraries
target_link_libraries(KalaWindow PRIVATE
	OpenGL::GL
	${EXT_SHARED_DIR}/Vulkan/vulkan-1.lib
)
	
# Expose version string
add_compile_definitions(
	PROGRAM_VERSION="${PROGRAM_VERSION}"
)

# Copy important files
set(COPY_TARGETS README.md LICENSE.md logo.png)
foreach(FILE ${COPY_TARGETS})
add_custom_command(TARGET KalaWindow POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -f 
		"$<TARGET_FILE_DIR:KalaWindow>/${FILE}"
    COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_SOURCE_DIR}/${FILE}"
		"$<TARGET_FILE_DIR:KalaWindow>/${FILE}"
)
endforeach()

# Copy entire docs folder
add_custom_command(TARGET KalaWindow POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        "$<TARGET_FILE_DIR:KalaWindow>/docs"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/docs"
        "$<TARGET_FILE_DIR:KalaWindow>/docs"
)