add_library(external INTERFACE)

set(GLFW_BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

add_library(glad SHARED glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

add_library(stb_image SHARED stb/stb_image.cpp)
target_include_directories(stb_image PUBLIC stb)

add_subdirectory(glm)

add_library(
    imgui SHARED
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_glfw.h
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_opengl3.h
    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
)
target_include_directories(imgui PUBLIC imgui imgui/backends)
target_link_libraries(imgui PRIVATE glfw)

set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(assimp)

target_link_libraries(external INTERFACE glfw glad stb_image glm::glm imgui assimp)
