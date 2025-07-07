cmake_minimum_required(VERSION 3.29.2)

project(KalaWindow VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Runtime lib handling for MSVC
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# === Platform Check ===
if (NOT WIN32)
    message(FATAL_ERROR "[KALAWINDOW] Vulkan/OpenGL renderer currently supports only Windows.")
endif()

# === Renderer Selection ===
if (NOT DEFINED KALAWINDOW_RENDERER)
    message(FATAL_ERROR "[KALAWINDOW] You must define -DKALAWINDOW_RENDERER=opengl or vulkan")
endif()

string(TOLOWER "${KALAWINDOW_RENDERER}" KALAWINDOW_RENDERER)
message(STATUS "[KALAWINDOW] Selected renderer: ${KALAWINDOW_RENDERER}")

set(ENABLE_OPENGL FALSE)
set(ENABLE_VULKAN FALSE)

if(KALAWINDOW_RENDERER STREQUAL "opengl")
    set(ENABLE_OPENGL TRUE)
elseif(KALAWINDOW_RENDERER STREQUAL "vulkan")
    set(ENABLE_VULKAN TRUE)
else()
    message(FATAL_ERROR "[KALAWINDOW] Invalid renderer: '${KALAWINDOW_RENDERER}'. Must be 'vulkan' or 'opengl'.")
endif()

# === Dependencies ===
if(ENABLE_VULKAN)
    find_package(Vulkan REQUIRED)
endif()

if(ENABLE_OPENGL)
    find_package(OpenGL REQUIRED)
endif()

# === Paths ===
set(WINDOW_SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(WINDOW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# === Source Files ===
set(WINDOW_SRC_FILES)

file(GLOB BASE_SRC CONFIGURE_DEPENDS
    ${WINDOW_SRC_DIR}/*.cpp
    ${WINDOW_SRC_DIR}/core/*.cpp
    ${WINDOW_SRC_DIR}/graphics/*.cpp
	${WINDOW_SRC_DIR}/graphics/opengl/*.cpp
	${WINDOW_SRC_DIR}/graphics/vulkan/*.cpp
    ${WINDOW_SRC_DIR}/windows/*.cpp
    ${WINDOW_SRC_DIR}/linux/*.cpp
)
list(APPEND WINDOW_SRC_FILES ${BASE_SRC})

if(ENABLE_OPENGL)
    file(GLOB OPENGL_SRC CONFIGURE_DEPENDS
        ${WINDOW_SRC_DIR}/opengl/*.cpp
    )
    list(APPEND WINDOW_SRC_FILES ${OPENGL_SRC})
endif()

if(ENABLE_VULKAN)
    file(GLOB VULKAN_SRC CONFIGURE_DEPENDS
        ${WINDOW_SRC_DIR}/vulkan/*.cpp
    )
    list(APPEND WINDOW_SRC_FILES ${VULKAN_SRC})
endif()

# === SHARED LIBRARY (.dll) ===
add_library(KalaWindow SHARED ${WINDOW_SRC_FILES})

# Enable C++ exception handling semantics for MSVC
if (MSVC)
    target_compile_options(KalaWindow PRIVATE /EHsc)
endif()

# Set output name with renderer suffix and debug postfix
set_target_properties(KalaWindow PROPERTIES
    OUTPUT_NAME "KalaWindow_${KALAWINDOW_RENDERER}$<$<CONFIG:Debug>:D>"
)

target_include_directories(KalaWindow PRIVATE ${WINDOW_INCLUDE_DIR})

target_compile_definitions(KalaWindow PRIVATE
    KALAWINDOW_DLL_EXPORT
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

# === Compile Definitions Per Renderer ===
if(ENABLE_OPENGL)
    target_compile_definitions(KalaWindow PRIVATE KALAWINDOW_SUPPORT_OPENGL=1)
endif()

if(ENABLE_VULKAN)
    target_compile_definitions(KalaWindow PRIVATE KALAWINDOW_SUPPORT_VULKAN=1)
endif()

# === Link Libraries Per Renderer ===
if(ENABLE_VULKAN)
    target_link_libraries(KalaWindow PRIVATE Vulkan::Vulkan)
endif()

if(ENABLE_OPENGL)
    target_link_libraries(KalaWindow PRIVATE OpenGL::GL opengl32)
endif()
