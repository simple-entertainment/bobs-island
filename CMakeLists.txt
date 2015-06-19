cmake_minimum_required(VERSION 3.2)

include(../simplicity/simplicity-common.cmake)

# Project
#########################
project(bobs-island)

# Policies
#########################
cmake_policy(SET CMP0015 NEW)

# Includes
#########################
include_directories(../simplicity/src/main/c++)
include_directories(../simplicity-bullet/src/bullet/c++)
include_directories(../simplicity-bullet/src/bullet/c++/bullet)
include_directories(../simplicity-bullet/src/main/c++)
include_directories(../simplicity-editor/src/main/c++)
include_directories(../simplicity-freeglut/dep/freeglut/include)
include_directories(../simplicity-freeglut/src/main/c++)
include_directories(../simplicity-live555/dep/live/BasicUsageEnvironment/include)
include_directories(../simplicity-live555/dep/live/groupsock/include)
include_directories(../simplicity-live555/dep/live/liveMedia/include)
include_directories(../simplicity-live555/dep/live/UsageEnvironment/include)
include_directories(../simplicity-live555/src/main/c++)
include_directories(../simplicity-opengl/dep/freeimageplus/include)
include_directories(../simplicity-opengl/dep/glew/include)
include_directories(../simplicity-opengl/src/main/c++)
include_directories(../simplicity-raknet/src/main/c++)
include_directories(../simplicity-raknet/src/raknet/c++)
include_directories(../simplicity-rocket/dep/librocket/include)
include_directories(../simplicity-rocket/src/main/c++)
include_directories(../simplicity-terrain/src/main/c++)
include_directories(src/main/c++)

link_directories(../simplicity)
link_directories(../simplicity-bullet)
link_directories(../simplicity-editor)
link_directories(../simplicity-freeglut)
link_directories(../simplicity-freeglut/dep/freeglut/lib)
link_directories(../simplicity-freeglut/dep/glew/lib)
link_directories(../simplicity-live555/dep/ffmpeg/lib)
link_directories(../simplicity-live555/dep/live/BasicUsageEnvironment/lib)
link_directories(../simplicity-live555/dep/live/groupsock/lib)
link_directories(../simplicity-live555/dep/live/liveMedia/lib)
link_directories(../simplicity-live555/dep/live/UsageEnvironment/lib)
link_directories(../simplicity-live555)
link_directories(../simplicity-opengl)
link_directories(../simplicity-opengl/dep/freeimageplus/lib)
link_directories(../simplicity-opengl/dep/glew/lib)
link_directories(../simplicity-raknet)
link_directories(../simplicity-rocket)
link_directories(../simplicity-rocket/dep/librocket/lib)
link_directories(../simplicity-terrain)

# Client Target
#########################
file(GLOB_RECURSE CLIENT_SRC_FILES src/main/c++/bobs-island/client/*.cpp src/main/c++/bobs-island/client/*.h)
add_executable(bobs-island-client ${CLIENT_SRC_FILES})
target_link_libraries(bobs-island-client simplicity-freeglut)
target_link_libraries(bobs-island-client simplicity-live555)
target_link_libraries(bobs-island-client simplicity-opengl)
target_link_libraries(bobs-island-client simplicity-raknet)
target_link_libraries(bobs-island-client simplicity)

# FFMPEG
target_link_libraries(bobs-island-client avcodec)
target_link_libraries(bobs-island-client theoraenc)
target_link_libraries(bobs-island-client theoradec)
target_link_libraries(bobs-island-client x264)
target_link_libraries(bobs-island-client avcodec)
target_link_libraries(bobs-island-client avutil)
target_link_libraries(bobs-island-client swresample)
target_link_libraries(bobs-island-client swscale)
target_link_libraries(bobs-island-client pthread)
target_link_libraries(bobs-island-client z)

# Live555
target_link_libraries(bobs-island-client liveMedia)
target_link_libraries(bobs-island-client groupsock)
target_link_libraries(bobs-island-client BasicUsageEnvironment)
target_link_libraries(bobs-island-client UsageEnvironment)

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
	# RakNet
	target_link_libraries(bobs-island-client Ws2_32)
ENDIF(WIN32)
IF(UNIX)
	# FreeGLUT
	target_link_libraries(bobs-island-client glut)
	# OpenGL
	target_link_libraries(bobs-island-client GLEW)
	target_link_libraries(bobs-island-client GLU)
	target_link_libraries(bobs-island-client GL)
	# RakNet
	target_link_libraries(bobs-island-client pthread)
ENDIF(UNIX)

# Server Target
#########################
file(GLOB_RECURSE SERVER_SRC_FILES src/main/c++/bobs-island/server/*.cpp src/main/c++/bobs-island/server/*.h)
add_executable(bobs-island-server ${SERVER_SRC_FILES})
target_link_libraries(bobs-island-server simplicity-bullet)
target_link_libraries(bobs-island-server simplicity-editor)
target_link_libraries(bobs-island-server simplicity-freeglut)
target_link_libraries(bobs-island-server simplicity-live555)
target_link_libraries(bobs-island-server simplicity-opengl)
target_link_libraries(bobs-island-server simplicity-raknet)
target_link_libraries(bobs-island-server simplicity-rocket)
target_link_libraries(bobs-island-server simplicity-terrain)
target_link_libraries(bobs-island-server simplicity)

# FFMPEG
target_link_libraries(bobs-island-server avcodec)
target_link_libraries(bobs-island-server theoraenc)
target_link_libraries(bobs-island-server theoradec)
target_link_libraries(bobs-island-server x264)
target_link_libraries(bobs-island-server avutil)
target_link_libraries(bobs-island-server swresample)
target_link_libraries(bobs-island-server swscale)
target_link_libraries(bobs-island-server pthread)
target_link_libraries(bobs-island-server z)

# libnoise
target_link_libraries(bobs-island-server noise)

# Live555
target_link_libraries(bobs-island-server liveMedia)
target_link_libraries(bobs-island-server groupsock)
target_link_libraries(bobs-island-server BasicUsageEnvironment)
target_link_libraries(bobs-island-server UsageEnvironment)

# OpenGL
target_link_libraries(bobs-island-server freeimageplus)
target_link_libraries(bobs-island-server freeimage)

IF(WIN32)
	# FreeGLUT
	target_link_libraries(bobs-island-server freeglut)
	# OpenGL
	target_link_libraries(bobs-island-server glew32)
	target_link_libraries(bobs-island-server GlU32)
	target_link_libraries(bobs-island-server OpenGL32)
	# PhysX
	# TODO - Requires /MT !!!
	# RakNet
	target_link_libraries(bobs-island-server Ws2_32)
	# libRocket
	target_link_libraries(bobs-island-server debug RocketCore_d)
	target_link_libraries(bobs-island-server optimized RocketCore)
ENDIF(WIN32)
IF(UNIX)
	# FreeGLUT
	target_link_libraries(bobs-island-server glut)
	# OpenGL
	target_link_libraries(bobs-island-server GLEW)
	target_link_libraries(bobs-island-server GLU)
	target_link_libraries(bobs-island-server GL)
	# RakNet
	target_link_libraries(bobs-island-server pthread)
	# libRocket
	target_link_libraries(bobs-island-server RocketCore)
ENDIF(UNIX)
