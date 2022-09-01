# the project's main CMakeLists file

cmake_minimum_required(VERSION 3.15)

project(Galunga)

set(CMAKE_CXX_STANDARD 20)

set(galunga_shared_source_files
	"src/GAssert.cpp"
	"src/utils/LoadFile.cpp"
)

set(galunga_shared_header_files
	"src/Platform.h"
	"src/GAssert.h"
	"src/Exception.h" 
	"src/utils/EventBus.h"
	"src/utils/LoadFile.h" 
	"src/utils/Timer.h" 
	"src/net/Address.h" 
	"src/net/NetEvents.h"
	"src/net/Player.h"
)

set(galunga_client_source_files
	"src/TextureManager.cpp" 
	"src/Renderer.cpp"
	"src/net/client/MainClient.cpp"
	"src/net/client/ApplicationClient.cpp" 
	"src/net/client/NetworkClient.cpp"
)

set(galunga_client_header_files
	"src/TextureManager.h" 
	"src/Renderer.h"
	"src/net/client/ApplicationClient.h"
	"src/net/client/NetworkClient.h"
)

set(galunga_server_source_files
	"src/net/server/MainServer.cpp"
	"src/net/server/NetworkServer.cpp"
	"src/net/server/ApplicationServer.cpp"
)

set(galunga_server_header_files
	"src/net/server/NetworkServer.h"
	"src/net/server/ApplicationServer.h"
)

add_executable(galunga_client
	${galunga_client_source_files}
	${galunga_client_header_files}
)

add_executable(galunga_server
	${galunga_server_source_files}
	${galunga_server_header_files}
)

add_library(galunga_common 
	${galunga_shared_source_files} 
	${galunga_shared_header_files}
)

# Determine whether we're compiling with clang++
string(FIND "${CMAKE_CXX_COMPILER}" "clang++" COMPILER_CLANGPP)
if(COMPILER_CLANGPP GREATER -1)
    set(COMPILER_CLANGPP 1)
else()
    set(COMPILER_CLANGPP 0)
endif()

target_compile_options(galunga_common PUBLIC
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

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT galunga_client)

find_package(OpenGL REQUIRED)

# enable asan for debug builds
if (DEBUG)
    if (WIN32)
        target_compile_options(galunga_common PUBLIC /fsanitize=address)
    else()
        target_compile_options(galunga_common PUBLIC -fsanitize=address)
    endif()
endif()

add_subdirectory(external)

target_include_directories(galunga_common PUBLIC
	src
	vendor
	${enet_SOURCE_DIR}/include # dumb hack
)

target_link_libraries(galunga_client fwog glm glfw lib_glad EnTT::EnTT stb galunga_common imgui cereal)
target_link_libraries(galunga_server glm EnTT::EnTT galunga_common)

add_custom_target(copy_assets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data/assets ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(galunga_client copy_assets)