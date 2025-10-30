cmake_minimum_required(VERSION 3.10)
project(openxr_layer_minimal C)

# Layer configuration
set(LAYER_NAME        "XR_APILAYER_minimal"        CACHE STRING "Layer name")
set(LAYER_DESCRIPTION "A minimal OpenXR API layer" CACHE STRING "Layer description")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build shared library
add_library(${LAYER_NAME} SHARED)

# Fetch OpenXR SDK and build only headers
include(FetchContent)
FetchContent_Declare(
	OpenXR
	GIT_REPOSITORY https://github.com/KhronosGroup/OpenXR-SDK-Source.git
	GIT_TAG release-1.1.53
	GIT_SHALLOW TRUE
)
FetchContent_GetProperties(OpenXR)
if(NOT openxr_POPULATED)
	FetchContent_Populate(OpenXR)
	# Manually add just the include directory to build headers
	# Set required variables for the header generation
	set(PROJECT_SOURCE_DIR ${openxr_SOURCE_DIR})
	set(PROJECT_BINARY_DIR ${openxr_BINARY_DIR})
	find_package(Python3 REQUIRED COMPONENTS Interpreter)
	set(CODEGEN_PYTHON_PATH "${openxr_SOURCE_DIR}/specification/scripts:${openxr_SOURCE_DIR}/src/scripts:$ENV{PYTHONPATH}")
	set(CODEGEN_FOLDER "Generated")
	add_subdirectory(${openxr_SOURCE_DIR}/include ${openxr_BINARY_DIR}/include)
endif()
target_link_libraries(${LAYER_NAME} PRIVATE OpenXR::headers)

# Platform
if(WIN32)
	set(XR_USE_PLATFORM_WIN32 ON)
	set(_CRT_SECURE_NO_WARNINGS ON)
elseif(UNIX AND NOT APPLE)
	set(XR_USE_PLATFORM_XLIB ON)
	target_link_libraries(${LAYER_NAME} PRIVATE pthread dl)
else()
	message(FATAL_ERROR "Unsupported platform")
endif()


target_include_directories(${LAYER_NAME} PRIVATE
	${openxr_SOURCE_DIR}/src/common
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_BINARY_DIR}  # For generated layer_config.h
)
target_sources(${LAYER_NAME} PRIVATE
	src/layer.c
	src/dispatch_core.c
	src/dispatch.c
	src/generated/layer_gen.c
)

# OpenXR-specific defines
target_compile_definitions(${LAYER_NAME} PRIVATE
	XR_NO_PROTOTYPES  # We get function pointers at runtime
)

# Generate templated resources
set(LAYER_LIBRARY_NAME "$<TARGET_FILE_NAME:${LAYER_NAME}>")
set(LAYER_LIBRARY_PATH "$<TARGET_FILE:${LAYER_NAME}>")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/layer_config.h.in   ${CMAKE_CURRENT_BINARY_DIR}/layer_config.h     @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/manifest.json.in    ${CMAKE_CURRENT_BINARY_DIR}/${LAYER_NAME}.json @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/enable-layer.py.in  ${CMAKE_CURRENT_BINARY_DIR}/enable-layer.py    @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/disable-layer.py.in ${CMAKE_CURRENT_BINARY_DIR}/disable-layer.py   @ONLY)

# Platform-specific quick scripts
if(WIN32)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/enable-layer.bat.in ${CMAKE_CURRENT_BINARY_DIR}/enable-layer.bat @ONLY)
elseif(UNIX)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/enable-layer.sh.in  ${CMAKE_CURRENT_BINARY_DIR}/enable-layer.sh  @ONLY)
	# Make shell scripts executable
	file(CHMOD
		${CMAKE_CURRENT_BINARY_DIR}/enable-layer.sh
		${CMAKE_CURRENT_BINARY_DIR}/enable-layer.py
		${CMAKE_CURRENT_BINARY_DIR}/disable-layer.py
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
					GROUP_READ GROUP_EXECUTE
					WORLD_READ WORLD_EXECUTE)
endif()

# Install targets
install(TARGETS ${LAYER_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LAYER_NAME}.json
	DESTINATION share/openxr/api_layers
)

# Print configuration
message(STATUS "Layer name: ${LAYER_NAME}")
message(STATUS "OpenXR SDK: ${openxr_SOURCE_DIR}")
message(STATUS "")
message(STATUS "Layer management scripts generated in build directory:")
if(WIN32)
	message(STATUS "  ${CMAKE_CURRENT_BINARY_DIR}/enable-layer.bat  (quick - implicit layer)")
endif()
if(UNIX)
	message(STATUS "  ${CMAKE_CURRENT_BINARY_DIR}/enable-layer.sh   (quick - implicit layer)")
endif()
message(STATUS "  ${CMAKE_CURRENT_BINARY_DIR}/enable-layer.py   (full-featured)")
message(STATUS "  ${CMAKE_CURRENT_BINARY_DIR}/disable-layer.py  (uninstall)")
