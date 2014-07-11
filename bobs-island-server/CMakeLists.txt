cmake_minimum_required(VERSION 2.8)

include(../../simplicity/simplicity/simplicity-common.cmake)

# Project
project(bobs-island-server)

# Policies
cmake_policy(SET CMP0015 NEW)

# Paths
file(GLOB_RECURSE SRC_FILES src/main/c++/*.cpp src/main/c++/*.h)
include_directories(../../bobs-island/bobs-island/src/main/c++)
include_directories(../../simplicity/simplicity/src/main/c++)
include_directories(../../simplicity-bullet/simplicity-bullet/src/bullet/c++)
include_directories(../../simplicity-bullet/simplicity-bullet/src/bullet/c++/bullet)
include_directories(../../simplicity-bullet/simplicity-bullet/src/main/c++)
include_directories(../../simplicity-raknet/simplicity-raknet/src/main/c++)
include_directories(../../simplicity-raknet/simplicity-raknet/src/raknet/c++)
include_directories(../../the-island/the-island/src/main/c++)
include_directories(src/main/c++)

link_directories(../../bobs-island/bobs-island)
link_directories(../../simplicity/simplicity)
link_directories(../../simplicity-bullet/simplicity-bullet)
link_directories(../../simplicity-raknet/simplicity-raknet)
link_directories(../../the-island/the-island)

# Output
add_executable(bobs-island-server ${SRC_FILES})
target_link_libraries(bobs-island-server bobs-island)
target_link_libraries(bobs-island-server the-island)
target_link_libraries(bobs-island-server simplicity-bullet)
target_link_libraries(bobs-island-server simplicity-raknet)
target_link_libraries(bobs-island-server simplicity)

