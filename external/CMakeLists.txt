# external content definitions
include(FetchContent)

option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)
option(GLFW_BUILD_EXAMPLES "" OFF)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG        3.3.2
    GIT_SHALLOW ON
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG        cc98465e3508535ba8c7f6208df934c156a018dc
    GIT_SHALLOW ON
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG        v1.84.2
    GIT_SHALLOW ON
)

option(FWOG_BUILD_EXAMPLES "" off)
FetchContent_Declare(
    fwog
    GIT_REPOSITORY https://github.com/JuanDiegoMontoya/Fwog.git
    GIT_TAG        33a0e0622bcd10f49a59f33c2ae06938723935c0
    GIT_SHALLOW ON
)

FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG        85ca2f356234de3c945667d12857dea2a26c214d
    GIT_SHALLOW ON
)

FetchContent_Declare(
    enet
    GIT_REPOSITORY https://github.com/JuanDiegoMontoya/enet.git
    GIT_TAG        a683bb83dde5552871764f53f6a8f90d8d2e3d52
    GIT_SHALLOW ON
)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/JuanDiegoMontoya/stb.git
    GIT_TAG        53a36967a8012590940122fc2e2117cc8f7eb34c
    GIT_SHALLOW ON
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)

FetchContent_MakeAvailable(glm glfw fwog EnTT enet)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)

    add_library(lib_imgui
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)

    target_include_directories(lib_imgui PUBLIC 
        ${imgui_SOURCE_DIR} 
        ${imgui_SOURCE_DIR}/backends
        ${glfw_SOURCE_DIR}/include)

    target_link_libraries(lib_imgui PRIVATE glfw)
endif()

# stb does not have a CMakelists.txt, so we need to add our own target
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif()

target_compile_definitions(glm INTERFACE GLM_FORCE_SILENT_WARNINGS)