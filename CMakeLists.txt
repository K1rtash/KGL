cmake_minimum_required(VERSION 3.16)
project(opengl)

set(CMAKE_CXX_STANDARD 23)

set(GLAD_DIR "" CACHE PATH "Path to GLAD")
set(GLFW_DIR "" CACHE PATH "Path to GLFW")
set(GLM_DIR  "" CACHE PATH "Path to GLM")
set(ASSIMP_DIR "" CACHE PATH "Path to Assimp")

option(KGL_COPY_ASSETS "Copia la carpeta assets dentro de build" OFF)

# Esto es lo único que necesitas para que funcione find_package
list(APPEND CMAKE_PREFIX_PATH ${GLFW_DIR})
list(APPEND CMAKE_PREFIX_PATH ${ASSIMP_DIR})

find_package(glfw3 REQUIRED CONFIG)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED CONFIG)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${GLAD_DIR}/include 
    ${GLM_DIR}/include 
)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cxx)
file(GLOB EXT_SRC_FILES ${PROJECT_SOURCE_DIR}/src/ext/*)

add_executable(${PROJECT_NAME} 
    ${SRC_FILES} 
    ${EXT_SRC_FILES}
    ${GLAD_DIR}/src/glad.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw 
    OpenGL::GL
    assimp::assimp
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

if(KGL_COPY_ASSETS)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    )
endif()