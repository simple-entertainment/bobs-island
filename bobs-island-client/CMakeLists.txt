cmake_minimum_required(VERSION 2.8)

include(../../simplicity/simplicity/simplicity-common.cmake)

# Project
project(bobs-island-client)

# Policies
cmake_policy(SET CMP0015 NEW)

# Paths
file(GLOB_RECURSE SRC_FILES src/main/c++/*.cpp src/main/c++/*.h)
include_directories(../../bobs-island/bobs-island/src/main/c++)
include_directories(../../simplicity/simplicity/src/main/c++)
include_directories(../../simplicity-bullet/simplicity-bullet/src/bullet/c++)
include_directories(../../simplicity-bullet/simplicity-bullet/src/bullet/c++/bullet)
include_directories(../../simplicity-bullet/simplicity-bullet/src/main/c++)
include_directories(../../simplicity-freeglut/simplicity-freeglut/dep/freeglut/include)
include_directories(../../simplicity-freeglut/simplicity-freeglut/src/main/c++)
include_directories(../../simplicity-opengl/simplicity-opengl/dep/freeimageplus/include)
include_directories(../../simplicity-opengl/simplicity-opengl/dep/glew/include)
include_directories(../../simplicity-opengl/simplicity-opengl/src/main/c++)
include_directories(../../simplicity-physx/simplicity-physx/dep/physx/include)
include_directories(../../simplicity-physx/simplicity-physx/src/main/c++)
include_directories(../../simplicity-raknet/simplicity-raknet/src/main/c++)
include_directories(../../simplicity-raknet/simplicity-raknet/src/raknet/c++)
include_directories(../../simplicity-rocket/simplicity-rocket/dep/librocket/include)
include_directories(../../simplicity-rocket/simplicity-rocket/src/main/c++)
include_directories(../../the-island/the-island/src/main/c++)
include_directories(src/main/c++)

link_directories(../../bobs-island/bobs-island)
link_directories(../../simplicity/simplicity)
link_directories(../../simplicity-bullet/simplicity-bullet)
link_directories(../../simplicity-freeglut/simplicity-freeglut)
link_directories(../../simplicity-freeglut/simplicity-freeglut/dep/freeglut/lib)
link_directories(../../simplicity-freeglut/simplicity-freeglut/dep/glew/lib)
link_directories(../../simplicity-opengl/simplicity-opengl)
link_directories(../../simplicity-opengl/simplicity-opengl/dep/freeimageplus/lib)
link_directories(../../simplicity-opengl/simplicity-opengl/dep/glew/lib)
#link_directories(../../simplicity-physx/simplicity-physx)
#link_directories(../../simplicity-physx/simplicity-physx/dep/physx/lib)
link_directories(../../simplicity-raknet/simplicity-raknet)
link_directories(../../simplicity-rocket/simplicity-rocket)
link_directories(../../simplicity-rocket/simplicity-rocket/dep/librocket/lib)
link_directories(../../the-island/the-island)

# Output
add_executable(bobs-island-client ${SRC_FILES})
target_link_libraries(bobs-island-client bobs-island)
target_link_libraries(bobs-island-client the-island)
target_link_libraries(bobs-island-client simplicity-bullet)
target_link_libraries(bobs-island-client simplicity-freeglut)
target_link_libraries(bobs-island-client simplicity-opengl)
#target_link_libraries(bobs-island-client simplicity-physx)
target_link_libraries(bobs-island-client simplicity-raknet)
target_link_libraries(bobs-island-client simplicity-rocket)
target_link_libraries(bobs-island-client simplicity)

# OpenGL
target_link_libraries(bobs-island-client freeimageplus)
target_link_libraries(bobs-island-client freeimage)

IF(WIN32)
	# FreeGLUT
	target_link_libraries(bobs-island-client freeglut)
	# OpenGL
	target_link_libraries(bobs-island-client glew32)
	target_link_libraries(bobs-island-client GlU32)
	target_link_libraries(bobs-island-client OpenGL32)
	# PhysX
	# TODO - Requires /MT !!!
	# RakNet
	target_link_libraries(bobs-island-client Ws2_32)
	# libRocket
	target_link_libraries(bobs-island-client debug RocketCore_d)
	target_link_libraries(bobs-island-client optimized RocketCore)
ENDIF(WIN32)
IF(UNIX)
	# FreeGLUT
	target_link_libraries(bobs-island-client glut)
	# OpenGL
	target_link_libraries(bobs-island-client GLEW)
	target_link_libraries(bobs-island-client GLU)
	target_link_libraries(bobs-island-client GL)
	# PhysX
	#target_link_libraries(bobs-island-client debug PhysX3CommonCHECKED)
	#target_link_libraries(bobs-island-client debug PhysX3CHECKED)
	# RakNet
	target_link_libraries(bobs-island-client pthread)
	# libRocket
	target_link_libraries(bobs-island-client RocketCore)
ENDIF(UNIX)

