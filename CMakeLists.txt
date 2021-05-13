# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.18)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

project ("LyricMidi")

include_directories("${CMAKE_SOURCE_DIR}/LyricMidi/Public")
include_directories("${CMAKE_SOURCE_DIR}/LyricMidiPlayer")

include_directories(${LyricMidiPlayer_INCLUDE_DIRS})
link_directories("${CMAKE_SOURCE_DIR}/LyricMidiPlayer/external/glfw/lib")

add_subdirectory ("LyricMidi")
add_subdirectory ("LyricMidiPlayer")

add_subdirectory ("MidiDev")
target_link_libraries(MidiDev PRIVATE LyricMidi)
target_link_libraries(MidiDev PRIVATE LyricMidiPlayer)
