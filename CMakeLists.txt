cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(LearningOpenGL)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(my_app src/main.cpp)

find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(my_app glfw)
target_link_libraries(my_app GLEW::GLEW)

find_package(OpenGL REQUIRED)

target_link_libraries(my_app OpenGL::GL)

