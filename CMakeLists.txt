# the project's main CMakeLists file

cmake_minimum_required(VERSION 3.14)

project(Galunga)

set(CMAKE_CXX_STANDARD 20)

set(galunga_source_files
	src/main.cpp
	src/Renderer.cpp
	src/Application.cpp)

set(galunga_header_files
	include/Renderer.h
	include/Application.h
)

add_executable(galunga ${galunga_source_files} ${galunga_header_files})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT galunga)
target_include_directories(galunga PRIVATE include vendor)

find_package(OpenGL REQUIRED)

# enable asan for debug builds
if (DEBUG)
    if (WIN32)
        target_compile_options(galunga PUBLIC /fsanitize=address)
    else()
        target_compile_options(galunga PUBLIC -fsanitize=address)
    endif()
endif()

# Determine whether we're compiling with clang++
string(FIND "${CMAKE_CXX_COMPILER}" "clang++" COMPILER_CLANGPP)
if(COMPILER_CLANGPP GREATER -1)
    set(COMPILER_CLANGPP 1)
else()
    set(COMPILER_CLANGPP 0)
endif()

target_compile_options(galunga
	INTERFACE
	$<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>,${COMPILER_CLANGPP}>:
	-Wall
	-Wextra
	-pedantic-errors
	-Wno-missing-field-initializers
	-Wno-unused-result
	>
	$<$<CXX_COMPILER_ID:MSVC>:
	/W4
	/WX
	/permissive-
	/wd4324 # structure was padded
	>
)


option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if(${FORCE_COLORED_OUTPUT})
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif()
endif()

add_subdirectory(external)

target_include_directories(galunga PRIVATE ${enet_SOURCE_DIR}/include) # dumb hack
target_link_libraries(galunga fwog glfw lib_glad EnTT::EnTT stb)

add_custom_target(copy_assets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data/assets ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(galunga copy_assets)