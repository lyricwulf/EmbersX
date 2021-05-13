#pragma once

#pragma region[Includes]

#include <cinttypes>
#include <memory>
#include <thread>
#include <vector>

#include "MidiBuffer.h"
#include "MidiMap.h"
#include "MidiTrack.h"

#pragma endregion

enum FileFormat {
  SINGLE_TRACK = 0x0000,  // Type 0 MIDI file
  MULTI_TRACK = 0x0001,   // Type 1 MIDI File
  MULTI_SONG = 0x0002     // Series of Type 0 MIDI files
};

namespace midi {
class File {
#pragma region[Managers]
 public:
  File(const char* Path);
  ~File();

#pragma endregion

#pragma region[Public]
 public:
  bool Good() const;
  void Ready();

  void DebugOutput(std::ostream& Out) const;

  Map* operator[](uint8_t i) const { return master_maps[i]; }
  uint32_t duration = 0;
  uint16_t ppqn = 96;

#pragma endregion

#pragma region[Private]
 public:
  // Data members
  midi::Buffer buffer;
  // Header attributes
  // header_chunk = "MThd" + <header_length> + <format> + <n> + <division>
  bool HasValidHeader() const;
  Iter HeaderHead() const;

  FileFormat format;
  uint16_t track_count = 0;

  Lane* master_lanes[129];
  Map* master_maps[129];

  std::vector<Track*> tracks;
  void ParseTracks(std::istream& stream);
  // Starts async construction of Track
  // Returns the length of the track
  std::vector<std::thread*> threads;
  uint32_t DispatchTrack(uint32_t track_number, Iter start);

#pragma endregion
};

}  // namespace midi