project(Simulation)
cmake_minimum_required(VERSION 2.8)

################################
# Main project
################################
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

# Includes
include_directories(Include)

#OpenGL
TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL2 GL GLU SDL2 SDL2_image GLEW)
#Oculus
TARGET_LINK_LIBRARIES(${PROJECT_NAME} udev ovr pthread X11 Xinerama Xrandr)
#Boost
TARGET_LINK_LIBRARIES(${PROJECT_NAME} boost_program_options)

set(CMAKE_CXX_FLAGS "-std=c++14 -g -Wall -Wextra")
