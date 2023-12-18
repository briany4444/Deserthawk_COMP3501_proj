cmake_minimum_required(VERSION 2.6)

# Name of project
set(PROJ_NAME "CameraDemo")
project(${PROJ_NAME})

# Specify project files: header files and source files
set(HDRS
    asteroid.h player.h camera.h game.h orb.h resource.h resource_manager.h scene_graph.h scene_node.h spaceship.h terrain.h model_loader.h
    tree.h thorn.h light.h Ui.h
)
 
set(SRCS
   asteroid.cpp player.cpp camera.cpp game.cpp main.cpp orb.cpp resource.cpp tree.cpp thorn.cpp resource_manager.cpp scene_graph.cpp scene_node.cpp spaceship.cpp Ui.cpp
   material_vp.glsl material_fp.glsl terrain.cpp firefly_particle_vp.glsl firefly_particle_fp.glsl firefly_particle_gp.glsl light.cpp ui_vp.glsl screen_space_vp.glsl screen_space_fp.glsl
)

# Add path name to configuration file
configure_file(path_config.h.in path_config.h)

# Add executable based on the header and source files
add_executable(${PROJ_NAME} ${HDRS} ${SRCS})

# Require OpenGL library
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} ${OPENGL_gl_LIBRARY})

# Other libraries needed
#set(LIBRARY_PATH C:/libs/Libraries)
set(LIBRARY_PATH "C:\OpenGL_Libraries\Libraries")
#set(LIBRARY_PATH C:\Users\User\Libraries) # Brian's Libraries
#set(LIBRARY_PATH D:/COMP\ 3501\ Libraries/Libraries) #Dylan's Libraries

include_directories(${LIBRARY_PATH}/include)
if(NOT WIN32)
    find_library(GLEW_LIBRARY GLEW)
    find_library(GLFW_LIBRARY glfw)
    find_library(SOIL_LIBRARY SOIL)
elseif(WIN32)
    find_library(GLEW_LIBRARY glew32s HINTS ${LIBRARY_PATH}/lib)
    find_library(GLFW_LIBRARY glfw3 HINTS ${LIBRARY_PATH}/lib)
    find_library(SOIL_LIBRARY SOIL HINTS ${LIBRARY_PATH}/lib)
endif(NOT WIN32)
target_link_libraries(${PROJ_NAME} ${GLEW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${GLFW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${SOIL_LIBRARY})

# The rules here are specific to Windows Systems
if(WIN32)
    # Avoid ZERO_CHECK target in Visual Studio
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
 
    # This will use the proper libraries in debug mode in Visual Studio
    set_target_properties(${PROJ_NAME} PROPERTIES DEBUG_POSTFIX _d)
endif(WIN32)

