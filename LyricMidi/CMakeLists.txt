# CMakeList.txt : CMake project for LyricMidi, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
# Add source to this project's executable.

file(GLOB_RECURSE SOURCES
    Public/*.h
    Private/*.cpp
)

add_library ( LyricMidi ${SOURCES} )
target_compile_features(LyricMidi PRIVATE cxx_std_17)

#set(BOOST_ROOT "C:/Boost") # either set it here or from the command line  
#set(Boost_USE_STATIC_LIBS OFF) 
#set(Boost_USE_MULTITHREADED ON)  
#set(Boost_USE_STATIC_RUNTIME OFF) 
#find_package(Boost 1.76.0 REQUIRED) 
#
#target_include_directories(LyricMidi PUBLIC ${Boost_INCLUDE_DIRS}) 
#target_link_libraries(LyricMidi ${Boost_LIBRARIES})
# TODO: Add tests and install targets if needed.