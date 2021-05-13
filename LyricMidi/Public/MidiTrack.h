#pragma once

#include <ostream>
#include <thread>
#include <vector>

#include "MidiBuffer.h"
#include "MidiEvent.h"
#include "MidiLane.h"
#include "MidiTypes.h"

using std::istream;
using std::thread;
using std::unique_ptr;

namespace midi {
class Track {
#pragma region[Managers]
 public:
  Track(uint32_t number, istream& fs, uint32_t length);
  Track(uint32_t number, Iter head, Iter end);
  Track(uint32_t number, Iter head, uint32_t length);

#pragma endregion

#pragma region[Private]
 private:
  Buffer buffer;
#pragma endregion

#pragma region[Public]
 public:
  void DebugOutput(std::ostream& Out);
  void Ready() const;

  size_t Size() const { return buffer.size(); };

  Lane* operator[](size_t i) { return &(lanes[i]); };

  uint32_t Duration;

#pragma endregion

#pragma region[Private]
 private:
  thread* thread;
  Iter Head;
  Iter End;
  uint32_t length;
  uint32_t Number;
  uint8_t channel_addend = 0;

  // 128 for notes and 1 for tempo
  Lane lanes[129];

  bool ParseEvents();
#pragma endregion
};
}  // namespace midi