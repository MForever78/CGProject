cmake_minimum_required (VERSION 2.8.9)

project(Experiment6)

SET(CMAKE_C_FLAGS "-Wno-deprecated-declarations")

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})

add_executable(Experiment6 main.c)

target_link_libraries(Experiment6 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
