# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
# Include sub-projects.

file(GLOB_RECURSE SOURCES
    *.h
    *.cpp
)

add_executable (MidiDev ${SOURCES}) 

