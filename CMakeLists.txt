cmake_minimum_required(VERSION 3.2)

include(lib/simplicity/simplicity-common.cmake)

# Project
#########################
project(bobs-island)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)

add_subdirectory(lib/simplicity)
add_subdirectory(lib/simplicity-bullet)
add_subdirectory(lib/simplicity-freeglut)
add_subdirectory(lib/simplicity-glfw)
add_subdirectory(lib/simplicity-live555)
add_subdirectory(lib/simplicity-opengl)
add_subdirectory(lib/simplicity-raknet)
add_subdirectory(lib/simplicity-rocket)
add_subdirectory(lib/simplicity-terrain)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

# Source
#########################
file(GLOB_RECURSE CLIENT_SRC_FILES src/main/c++/bobs-island/client/*.cpp src/main/c++/bobs-island/client/*.h)
file(GLOB_RECURSE SERVER_SRC_FILES src/main/c++/bobs-island/server/*.cpp src/main/c++/bobs-island/server/*.h src/main/c++/bobs-island/server-main/Main.cpp)
file(GLOB_RECURSE SERVER_EDITOR_SRC_FILES src/main/c++/bobs-island/server/*.cpp src/main/c++/bobs-island/server/*.h)

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
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${PROJECT_SOURCE_DIR}/build/assets
				   COMMENT "bobs-island-server || Copying assets")
add_custom_command(TARGET bobs-island-server PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/main/glsl ${PROJECT_SOURCE_DIR}/build/src/main/glsl
				   COMMENT "bobs-island-server || Copying source")

# Server (in Editor) Target
#########################
add_library(bobs-island-server-for-editor SHARED ${SERVER_SRC_FILES})
target_compile_options(bobs-island-server-for-editor PUBLIC -rdynamic)

# Server (in Editor) Target Dependencies
#########################

# libnoise
target_link_libraries(bobs-island-server-for-editor noise)

# Simplicity
target_link_libraries(bobs-island-server-for-editor simplicity-bullet)
target_link_libraries(bobs-island-server-for-editor simplicity-glfw)
target_link_libraries(bobs-island-server-for-editor simplicity-live555)
target_link_libraries(bobs-island-server-for-editor simplicity-opengl)
target_link_libraries(bobs-island-server-for-editor simplicity-raknet)
target_link_libraries(bobs-island-server-for-editor simplicity-rocket)
target_link_libraries(bobs-island-server-for-editor simplicity-terrain)
target_link_libraries(bobs-island-server-for-editor simplicity)

# Server (in Editor) Target Resources
#########################
add_custom_command(TARGET bobs-island-server-for-editor PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${PROJECT_SOURCE_DIR}/build/assets
				   COMMENT "bobs-island-server-for-editor || Copying assets")
add_custom_command(TARGET bobs-island-server-for-editor PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/main/glsl ${PROJECT_SOURCE_DIR}/build/src/main/glsl
				   COMMENT "bobs-island-server-for-editor || Copying source")
