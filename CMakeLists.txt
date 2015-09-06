cmake_minimum_required(VERSION 3.2)

include(lib/simplicity/simplicity-common.cmake)

# Project
#########################
project(bobs-island)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)

set(SIMPLE_LINK_TYPE SHARED)
add_subdirectory(lib/simplicity)
add_subdirectory(lib/simplicity-bullet)
add_subdirectory(lib/simplicity-glfw)
add_subdirectory(lib/simplicity-opengl)
add_subdirectory(lib/simplicity-terrain)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

# Source
#########################
file(GLOB_RECURSE SRC_FILES src/main/c++/bobs-island/common/*.cpp src/generated/*.cpp)

# Standalone Target
#########################
add_executable(bobs-island ${SRC_FILES} src/main/c++/bobs-island/main/Main.cpp)
target_include_directories(bobs-island PUBLIC src/generated)
target_include_directories(bobs-island PUBLIC src/main/c++)

# Standalone Target Dependencies
#########################

# libnoise
target_link_libraries(bobs-island noise)

# Simplicity
target_link_libraries(bobs-island simplicity-bullet)
target_link_libraries(bobs-island simplicity-glfw)
target_link_libraries(bobs-island simplicity-opengl)
target_link_libraries(bobs-island simplicity-terrain)
target_link_libraries(bobs-island simplicity)

# Standalone Target Resources
#########################
add_custom_command(TARGET bobs-island PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${PROJECT_SOURCE_DIR}/build/assets)

# Module Target
#########################
add_library(bobs-island-module MODULE ${SRC_FILES})
target_include_directories(bobs-island-module PUBLIC src/main/c++)

# Module Target Dependencies
#########################

# libnoise
target_link_libraries(bobs-island-module noise)

# Simplicity
target_link_libraries(bobs-island-module simplicity-bullet)
target_link_libraries(bobs-island-module simplicity-glfw)
target_link_libraries(bobs-island-module simplicity-opengl)
target_link_libraries(bobs-island-module simplicity-terrain)
target_link_libraries(bobs-island-module simplicity)

# Module Target Resources
#########################
add_custom_command(TARGET bobs-island-module PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${PROJECT_SOURCE_DIR}/build/assets)
