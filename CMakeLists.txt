cmake_minimum_required(VERSION 3.2)

include(lib/simplicity/simplicity-common.cmake)

# Project
#########################
project(bobs-island)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(lib/simplicity)
add_subdirectory(lib/simplicity-bullet)
add_subdirectory(lib/simplicity-cef)
add_subdirectory(lib/simplicity-freeglut)
add_subdirectory(lib/simplicity-glfw)
add_subdirectory(lib/simplicity-live555)
add_subdirectory(lib/simplicity-opengl)
add_subdirectory(lib/simplicity-raknet)
add_subdirectory(lib/simplicity-rocket)
add_subdirectory(lib/simplicity-terrain)
add_subdirectory(lib/simplicity-editor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

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
target_link_libraries(bobs-island-server simplicity-glfw)
target_link_libraries(bobs-island-server simplicity-live555)
target_link_libraries(bobs-island-server simplicity-opengl)
target_link_libraries(bobs-island-server simplicity-raknet)
target_link_libraries(bobs-island-server simplicity-rocket)
target_link_libraries(bobs-island-server simplicity-terrain)
target_link_libraries(bobs-island-server simplicity)

# Server Target Resources
#########################
add_custom_command(TARGET bobs-island-server PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/assets
				   COMMENT "bobs-island-server || Copying assets")
add_custom_command(TARGET bobs-island-server PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/main/css ${PROJECT_BINARY_DIR}/src/main/css
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/main/glsl ${PROJECT_BINARY_DIR}/src/main/glsl
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/main/html ${PROJECT_BINARY_DIR}/src/main/html
				   COMMENT "bobs-island-server || Copying source")
