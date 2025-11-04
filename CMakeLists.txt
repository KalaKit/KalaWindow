cmake_minimum_required(VERSION 3.29.2)

set(PROGRAM_VERSION "KalaWindow 0.3 Alpha")
set(PROGRAM_VERSION_NUMBER 0.3.0.0)

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

# Get prefix
set(KALA_PREFIX "")
if (KALA_PRERELEASE)
	set(KALA_PREFIX "_")
endif()
	
# Get version
set(KALA_VERSION "${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}")
	
# Get suffix
if (IS_RELEASE)
	set(KALA_SUFFIX "")
else()
	set(KALA_SUFFIX "d")
endif()

# Set output name with renderer suffix and debug postfix
set_target_properties(KalaWindow PROPERTIES
	OUTPUT_NAME "${KALA_PREFIX}KalaWindow${KALA_VERSION}${KALA_SUFFIX}"
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
    LIB_EXPORT
    WIN32_LEAN_AND_MEAN
    NOMINMAX
	UNICODE
	_UNICODE
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
	
# Set Windows Details tab data
if (IS_RELEASE)
	if (KALA_PRERELEASE)
		set(FILEFLAGS "0x02") # Release + PreRelease
	else()
		set(FILEFLAGS "0x00") # Pure Release
	endif()
else()
	if (KALA_PRERELEASE)
		set(FILEFLAGS "0x03") # Debug + PreRelease
	else()
		set(FILEFLAGS "0x01") # Pure Debug
	endif()
endif()

set(FILETYPE 0x2)    # 0x1 - executable, 0x2 - dynamic-link library
set(FILESUBTYPE 0x0) # 0x0 = normal DLL, only change if you're making drivers/fonts

# Public fields displayed in Windows Explorer
set(FILE_DESCRIPTION  "Multimedia Framework Library")
set(FILE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")
set(PRODUCT_NAME      "KalaWindow")
set(PRODUCT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")
set(COMPANY_NAME      "Lost Empire Entertainment")
set(LEGAL_COPYRIGHT   "(C) 2025 Lost Empire Entertainment")
set(LEGAL_TRADEMARKS  "Trademark of Lost Empire Entertainment")
set(ORIGINAL_FILENAME "${KALA_PREFIX}KWnd${KALA_VERSION}${KALA_SUFFIX}")

# Private/optional fields
set(INTERNAL_NAME  "${ORIGINAL_FILENAME}")
set(COMMENTS       "${PRODUCT_NAME}")
set(PRIVATE_BUILD  "${PRODUCT_NAME}")
set(SPECIAL_BUILD  "${PRODUCT_NAME}")

# Generate version.rc
configure_file(
	${CMAKE_SOURCE_DIR}/version.rc.in
	${CMAKE_BINARY_DIR}/version.rc
	@ONLY
)

target_sources(KalaWindow PRIVATE "${CMAKE_BINARY_DIR}/version.rc")

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