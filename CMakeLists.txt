# Project definition
cmake_minimum_required(VERSION 3.1)
set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
project(Magpie)
set(GUI_NAME "MagpieGUI")

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(GUI_SOURCES "${SRC_DIR}/main.cpp" "${SRC_DIR}/input.cpp" "${SRC_DIR}/display.cpp")
set(LIB_SOURCES
"${SRC_DIR}/mat.cpp"
"${SRC_DIR}/vec.cpp"
"${SRC_DIR}/angle.cpp"
"${SRC_DIR}/scene.cpp"
"${SRC_DIR}/light.cpp"
"${SRC_DIR}/pathtracer/pathtracer.cpp"
"${SRC_DIR}/pathtracer/opencl_pathtracer.cpp")

# libMagpie
add_library(${PROJECT_NAME} ${LIB_SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
target_include_directories(${PROJECT_NAME} PRIVATE "${INCLUDE_DIR}")

# Executable definition and properties
add_executable(${GUI_NAME} ${GUI_SOURCES})
set_property(TARGET ${GUI_NAME} PROPERTY CXX_STANDARD 11)
# Set name of GUI executable to be "Magpie"
set_property(TARGET ${GUI_NAME} PROPERTY OUTPUT_NAME ${PROJECT_NAME})
target_include_directories(${GUI_NAME} PRIVATE "${SRC_DIR}")
target_include_directories(${GUI_NAME} PRIVATE "${INCLUDE_DIR}")
target_link_libraries(${GUI_NAME} PRIVATE ${PROJECT_NAME})

# SDL
set(SDL_DIR "${LIB_DIR}/SDL")
add_subdirectory(${SDL_DIR} "${CMAKE_BINARY_DIR}/SDL" EXCLUDE_FROM_ALL)
add_dependencies(${GUI_NAME} SDL2main)
add_dependencies(${GUI_NAME} SDL2-static)
include_directories("${SDL_DIR}/include")
target_include_directories(${GUI_NAME} PRIVATE "${SDL_DIR}/include")
target_link_libraries(${GUI_NAME} PRIVATE SDL2main)
target_link_libraries(${GUI_NAME} PRIVATE SDL2-static)

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${GUI_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${GUI_NAME} PRIVATE "glad")

# OpenCL
find_package(OpenCL)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCL_LIBRARIES})
# C++ bindings
target_include_directories(${PROJECT_NAME} PRIVATE "${LIB_DIR}/OpenCL")

# glm
set(GLM_DIR "${LIB_DIR}/glm")
add_subdirectory(${GLM_DIR})
target_include_directories(${GUI_NAME} PRIVATE "${GLM_DIR}/glm")
target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_DIR})
target_link_libraries(${GUI_NAME} PRIVATE "glm")

# stb_image
set(STB_DIR "${LIB_DIR}/stb_image")
add_library("stb_image" "${STB_DIR}/stb_image.cpp")
target_include_directories("stb_image" PRIVATE ${STB_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${STB_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE "stb_image")

# yaml-cpp
set(YAML_DIR "${LIB_DIR}/yaml-cpp")
add_subdirectory(${YAML_DIR})
target_include_directories("yaml-cpp" PRIVATE "${YAML_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${YAML_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE "yaml-cpp")