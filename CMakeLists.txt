cmake_minimum_required(VERSION 3.29.2)

project(KalaWindow VERSION 0.3 LANGUAGES C CXX)

# Adds a '_' prefix in front of the created binary files if set to TRUE
set(KALA_PRERELEASE TRUE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Runtime lib handling for MSVC
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Platform Detection
if (WIN32)
    message(STATUS "[KALAWINDOW] Platform = Windows")
elseif(UNIX)
    find_package(X11 REQUIRED)
    message(STATUS "[KALAWINDOW] Platform = Linux")
else()
    message(FATAL_ERROR "[KALAWINDOW] Unsupported platform. Must be Windows or Linux.")
endif()

# Dependencies
# find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)

# Paths
set(WINDOW_SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(WINDOW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(EXT_SHARED_DIR "${CMAKE_SOURCE_DIR}/_external_shared")

# Source Files
set(WINDOW_SRC_FILES)

file(GLOB WINDOW_SRC CONFIGURE_DEPENDS
    ${WINDOW_SRC_DIR}/*.cpp
    ${WINDOW_SRC_DIR}/core/*.cpp
    ${WINDOW_SRC_DIR}/graphics/*.cpp
	${WINDOW_SRC_DIR}/graphics/opengl/*.cpp
	${WINDOW_SRC_DIR}/graphics/vulkan/*.cpp
    ${WINDOW_SRC_DIR}/windows/*.cpp
    ${WINDOW_SRC_DIR}/linux/*.cpp
	${WINDOW_SRC_DIR}/opengl/*.cpp
	${WINDOW_SRC_DIR}/vulkan/*.cpp
	
	#External stuff
	${EXT_SHARED_DIR}/imgui/*.cpp
	${EXT_SHARED_DIR}/imgui/backends/*.cpp
)
list(APPEND WINDOW_SRC_FILES ${WINDOW_SRC})

# SHARED LIBRARY (.dll)
add_library(KalaWindow SHARED ${WINDOW_SRC_FILES})

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
set(KALA_SUFFIX "$<$<CONFIG:Debug>:d>")

# Set output name with renderer suffix and debug postfix
set_target_properties(KalaWindow PROPERTIES
	OUTPUT_NAME "${KALA_PREFIX}KWnd${KALA_VERSION}${KALA_SUFFIX}"
)

file(GLOB_RECURSE HEADERS configure_depends
	"${CMAKE_SOURCE_DIR}/include/*.hpp"
)
target_sources(KalaWindow PRIVATE ${HEADERS})
target_include_directories(KalaWindow PRIVATE 
	${WINDOW_INCLUDE_DIR}
	${EXT_SHARED_DIR}
)

target_compile_definitions(KalaWindow PRIVATE
    KALAWINDOW_DLL_EXPORT
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

# Link libraries
target_link_libraries(KalaWindow PRIVATE
	Vulkan::Vulkan
	opengl32)
	#OpenGL::GL
	#Vulkan::Vulkan)
	
# Copy docs
set(COPY_TARGETS README.md LICENSE.md CHANGES.txt)
foreach(FILE ${COPY_TARGETS})
add_custom_command(TARGET KalaWindow POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove "$<TARGET_FILE_DIR:KalaWindow>/${FILE}"
    COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_SOURCE_DIR}/${FILE}"
		"$<TARGET_FILE_DIR:KalaWindow>/${FILE}"
)
endforeach()
