cmake_minimum_required(VERSION 3.16)
project(opengl)

set(CMAKE_CXX_STANDARD 23)

set(GLAD_DIR "C:/DEV/SDK/Headers/glad")
set(GLFW_DIR "C:/DEV/SDK/Static/GLFW")
set(GLM_DIR "C:/DEV/SDK/Static/glm")
set(JSON_DIR "C:/DEV/SDK/Headers/nlohmann")

# Esto es lo único que necesitas para que funcione find_package
list(APPEND CMAKE_PREFIX_PATH ${GLFW_DIR})
list(APPEND CMAKE_PREFIX_PATH ${GLAD_DIR})

find_package(glfw3 REQUIRED CONFIG)
find_package(OpenGL REQUIRED)
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${GLAD_DIR}/include ${GLM_DIR}/include ${JSON_DIR}/include
)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES} ${GLAD_DIR}/src/glad.c)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL)

#file(COPY ${CMAKE_SOURCE_DIR}/shaders
#     DESTINATION ${CMAKE_SOURCE_DIR})

#add_custom_command(
#    TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#            ${CMAKE_SOURCE_DIR}/shaders
#            $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
#)