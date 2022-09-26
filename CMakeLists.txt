# the project's main CMakeLists file

cmake_minimum_required(VERSION 3.15)

project(Galunga)

set(CMAKE_CXX_STANDARD 20)

set(galunga_shared_source_files
	"src/shared/GAssert.cpp"
	"src/shared/utils/LoadFile.cpp"
	"src/shared/Platform.cpp"
	"src/shared/ecs/Entity.cpp" 
	"src/shared/ecs/Scene.cpp"
	"src/shared/ecs/systems/System.cpp"
	"src/shared/ecs/systems/core/LifetimeSystem.cpp"
	"src/shared/ecs/systems/core/CollisionSystem.cpp"
	"src/shared/ecs/systems/game/PlayerSystem.cpp"
	"src/shared/ecs/systems/game/BulletSystem.cpp"
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
	"src/shared/ecs/Entity.h"
	"src/shared/ecs/Scene.h"
	"src/shared/ecs/components/core/Lifetime.h"
	"src/shared/ecs/components/core/Tag.h"
	"src/shared/ecs/systems/core/LifetimeSystem.h"
	"src/shared/ecs/systems/System.h"
	"src/shared/ecs/components/core/Transform.h"
	"src/shared/ecs/components/core/Sprite.h"
	"src/shared/ecs/events/AddSprite.h"
	"src/shared/ecs/components/core/Collider.h"
	"src/shared/ecs/systems/core/CollisionSystem.h"
	"src/shared/ecs/events/Collision.h"
	"src/shared/ecs/components/game/Bullet.h"
	"src/shared/ecs/components/game/Player.h"
	"src/shared/ecs/components/game/Enemy.h"
	"src/shared/ecs/systems/game/PlayerSystem.h"
	"src/shared/ecs/events/game/PlayerEvents.h"
	"src/shared/ecs/events/AxisBindingBase.h"
	"src/shared/ecs/systems/game/BulletSystem.h"
)

set(galunga_client_source_files
	"src/client/TextureManager.cpp" 
	"src/client/Renderer.cpp"
	"src/client/main.cpp"
	"src/client/Application.cpp" 
	"src/client/net/Host.cpp"
	"src/client/Input.cpp"
	"src/client/ecs/systems/RenderingSystem.cpp"
	"src/client/ecs/systems/DebugSystem.cpp"
)

set(galunga_client_header_files
	"src/client/TextureManager.h" 
	"src/client/Renderer.h"
	"src/client/Application.h"
	"src/client/net/Host.h"
	"src/client/Input.h"
	"src/client/ecs/systems/RenderingSystem.h"
	"src/client/ecs/systems/DebugSystem.h"
	"src/client/ecs/components/DebugDraw.h"
)

set(galunga_server_source_files
	"src/server/main.cpp"
	"src/server/net/Host.cpp"
	"src/server/Application.cpp"
)

set(galunga_server_header_files
	"src/server/net/Host.h"
	"src/server/Application.h"
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

# enable roughly equivalent (and strict!) compiler warnings depending on the compiler
target_compile_options(galunga_common PUBLIC
	$<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>,${COMPILER_CLANGPP}>:
	-Wall
	-Wextra
	-pedantic-errors
	-Wno-missing-field-initializers
	-Wno-unused-result
	>
	$<$<CXX_COMPILER_ID:MSVC>:
	-W4
	-WX
	-permissive-
	-wd4324 # disable warning indicating that a structure was padded
	>
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT galunga_client)

find_package(OpenGL REQUIRED)

# enable asan for debug builds
if (DEBUG)
    target_compile_options(galunga_common PUBLIC -fsanitize=address)
endif()

add_subdirectory(external)

target_include_directories(galunga_common PUBLIC
	src
	src/shared
	vendor
	${enet_SOURCE_DIR}/include # dumb hack
)

target_link_libraries(galunga_common PUBLIC glm EnTT::EnTT cereal)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_link_libraries(galunga_client fwog  glfw lib_glad imgui stb galunga_common)
else()
	target_link_libraries(galunga_client fwog  glfw lib_glad imgui stb galunga_common tbb)
endif()
target_link_libraries(galunga_server galunga_common)

add_custom_target(copy_assets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data/assets ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(galunga_client copy_assets)
