cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
#cmake_policy(VERSION 2.8)

set(NAME LyricMidiPlayer)

project(${NAME})

# GLFW 3.1 and earlier don't support vulkan
#find_package(glfw3 3.2 REQUIRED)

include_directories(external/glfw/include)
include_directories(external)
include_directories(external/glm)
include_directories(external/gli)
include_directories(base)

link_directories(external/glfw/lib)

if(CMAKE_VERSION VERSION_LESS 3.7.0)
	message("CMake version doesn't natively support Vulkan package. Loading custom package file.")
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
endif()

OPTION(USE_D2D_WSI "Build the project using Direct to Display swapchain" OFF)

find_package(Vulkan REQUIRED)

include_directories(${Vulkan_INCLUDE_DIRS})

# Set preprocessor defines
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES")

#add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-std=c++11)


file(GLOB SOURCE *.cpp base/*.cpp)
file(GLOB HEADERS *.hpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

add_library(${NAME} main.cpp ${SOURCE} ${HEADERS})
target_link_libraries(${NAME} ${Vulkan_LIBRARIES} glfw)

#expose includes
set(${NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/external/glfw/include
                         ${PROJECT_SOURCE_DIR}/external
                         ${PROJECT_SOURCE_DIR}/external/glm
                         ${PROJECT_SOURCE_DIR}/external/gli
                         ${PROJECT_SOURCE_DIR}/base
                         ${Vulkan_INCLUDE_DIRS}
    CACHE INTERNAL "${NAME}: Include Directories" FORCE)