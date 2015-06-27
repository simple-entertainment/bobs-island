cmake_minimum_required(VERSION 3.2)

include(lib/simplicity/simplicity-common.cmake)

# Project
#########################
project(bobs-island)

add_subdirectory(lib/simplicity)
add_subdirectory(lib/simplicity-bullet)
add_subdirectory(lib/simplicity-cef)
add_subdirectory(lib/simplicity-freeglut)
add_subdirectory(lib/simplicity-live555)
add_subdirectory(lib/simplicity-opengl)
add_subdirectory(lib/simplicity-raknet)
add_subdirectory(lib/simplicity-rocket)
add_subdirectory(lib/simplicity-terrain)
add_subdirectory(lib/simplicity-editor)

# Source
#########################
file(GLOB_RECURSE CLIENT_SRC_FILES src/main/c++/bobs-island/client/*.cpp src/main/c++/bobs-island/client/*.h)
file(GLOB_RECURSE SERVER_SRC_FILES src/main/c++/bobs-island/server/*.cpp src/main/c++/bobs-island/server/*.h)

# Client Target
#########################
add_executable(bobs-island-client ${CLIENT_SRC_FILES})

# Client Target Dependencies
#########################

# Simplicity
target_link_libraries(bobs-island-client simplicity-freeglut)
target_link_libraries(bobs-island-client simplicity-live555)
target_link_libraries(bobs-island-client simplicity-opengl)
target_link_libraries(bobs-island-client simplicity-raknet)
target_link_libraries(bobs-island-client simplicity)

# Server Target
#########################
add_executable(bobs-island-server ${SERVER_SRC_FILES})

# Server Target Dependencies
#########################

# libnoise
target_link_libraries(bobs-island-server noise)

# Simplicity
target_link_libraries(bobs-island-server simplicity-bullet)
target_link_libraries(bobs-island-server simplicity-cef)
target_link_libraries(bobs-island-server simplicity-editor)
target_link_libraries(bobs-island-server simplicity-freeglut)
target_link_libraries(bobs-island-server simplicity-live555)
target_link_libraries(bobs-island-server simplicity-opengl)
target_link_libraries(bobs-island-server simplicity-raknet)
target_link_libraries(bobs-island-server simplicity-rocket)
target_link_libraries(bobs-island-server simplicity-terrain)
target_link_libraries(bobs-island-server simplicity)
