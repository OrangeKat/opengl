cmake_minimum_required(VERSION 3.0)
project(FireParticle)
set(CMAKE_CXX_STANDARD 11)

file(GLOB SRC "src/*.cpp")
include_directories(src)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(glm CONFIG REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)

include_directories(${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
link_directories(${GLFW_LIBRARY_DIRS} ${GLEW_LIBRARY_DIRS})

add_executable(FireParticle ${SRC})
target_link_libraries(FireParticle ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})
target_link_libraries(FireParticle glm::glm)
