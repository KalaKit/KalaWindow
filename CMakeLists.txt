cmake_minimum_required(VERSION 3.30.3)

# Ensure install path is set correctly
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/out/install")
endif()

if (WIN32)
    # Enable hot reload for MSVC
    if (POLICY CMP0141)
        cmake_policy(SET CMP0141 NEW)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    endif()
    # Explicitly set the MSVC toolset for Visual Studio
    if (MSVC)
        set(CMAKE_GENERATOR_TOOLSET "v143")
    endif()
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

# Set runtime library type for MSVC
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
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
if (WIN32)
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/release/KalaCrashHandler.lib")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/debug/KalaCrashHandlerD.lib")
	endif()
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/release/libKalaCrashHandler.so")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/debug/libKalaCrashHandlerD.so")
	endif()
endif()

# Add source files
file(GLOB_RECURSE WINDOW_SRC_FILES CONFIGURE_DEPENDS
    ${WINDOW_SRC_DIR}/*.cpp
)

# Build as a shared library (DLL)
add_library(KalaWindow SHARED ${WINDOW_SRC_FILES})
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
)

if (WIN32)
    target_link_libraries(KalaWindow PRIVATE
		${CRASH_LIBRARY_PATH}
		opengl32)
else()
	find_package(Wayland QUIET)
	find_package(X11 QUIET)
	if (Wayland_FOUND)
		target_link_libraries(KalaWindow PRIVATE
			${CRASH_LIBRARY_PATH}
			Wayland::Client)
	elseif (X11_FOUND)
		target_link_libraries(KalaWindow PRIVATE
			${CRASH_LIBRARY_PATH}
			${X11_LIBRARIES})
	endif()
endif()

# Define preprocessor directives
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
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
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	set(DLL_ORIGIN_DIR "${CMAKE_SOURCE_DIR}/files/external dlls/release")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
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