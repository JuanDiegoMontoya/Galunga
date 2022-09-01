# the project's main CMakeLists file

cmake_minimum_required(VERSION 3.15)

project(Galunga)

set(CMAKE_CXX_STANDARD 20)

set(galunga_shared_source_files
	"src/shared/GAssert.cpp"
	"src/shared/utils/LoadFile.cpp"
	"src/shared/Platform.cpp"
)

set(galunga_shared_header_files
	"src/shared/Platform.h"
	"src/shared/GAssert.h"
	"src/shared/Exception.h" 
	"src/shared/utils/EventBus.h"
	"src/shared/utils/LoadFile.h" 
	"src/shared/utils/Timer.h" 
	"src/shared/net/Address.h" 
	"src/shared/net/NetEvents.h"
	"src/shared/net/Player.h"
)

set(galunga_client_source_files
	"src/client/TextureManager.cpp" 
	"src/client/Renderer.cpp"
	"src/client/MainClient.cpp"
	"src/client/ApplicationClient.cpp" 
	"src/client/net/NetworkClient.cpp"
)

set(galunga_client_header_files
	"src/client/TextureManager.h" 
	"src/client/Renderer.h"
	"src/client/ApplicationClient.h"
	"src/client/net/NetworkClient.h"
)

set(galunga_server_source_files
	"src/server/MainServer.cpp"
	"src/server/net/NetworkServer.cpp"
	"src/server/ApplicationServer.cpp"
)

set(galunga_server_header_files
	"src/server/net/NetworkServer.h"
	"src/server/ApplicationServer.h"
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
	src/shared
	vendor
	${enet_SOURCE_DIR}/include # dumb hack
)

target_link_libraries(galunga_client fwog glm glfw lib_glad EnTT::EnTT stb galunga_common imgui cereal)
target_link_libraries(galunga_server glm EnTT::EnTT galunga_common cereal)

add_custom_target(copy_assets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data/assets ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(galunga_client copy_assets)