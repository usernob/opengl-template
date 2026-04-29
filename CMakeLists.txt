cmake_minimum_required(VERSION 3.20)

project(opengl-template)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_subdirectory(external)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL external)

target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE
        GLFW_INCLUDE_NONE
        ASSETS_PATH=$<IF:$<CONFIG:Debug>,"${CMAKE_CURRENT_SOURCE_DIR}/assets","./assets">
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "copying assets"
)
