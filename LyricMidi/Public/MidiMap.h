#pragma once

#include <algorithm>
#include <cinttypes>
#include <forward_list>
#include <iterator>
#include <thread>

#include "MidiTypes.h"

namespace midi {

// Once map per lane

struct alignas(32) Color {
  uint8_t r, g, b;
};

// TODO: Optimize memory of maps by storing a delta tick for consecutive vals
struct Map : std::vector<uint8_t> {
#pragma region[Managers]

  Map(size_t size, std::vector<Note>* lane) : std::vector<uint8_t>() {
    resize(size + 1);
    max_tick = size;
    worker = new std::thread(&Map::LoadEvents, this, lane);
  }
  Map(size_t size, uint8_t number) : std::vector<uint8_t>() {
    reserve(size);
    debugnumber = number;
  }

#pragma endregion

#pragma region[Public]

  void DebugOutput(std::ostream& Out) const;
  void Ready();
  int debugnumber = 0;

#pragma endregion

#pragma region[Private]

 private:
  Map::iterator It(size_t i) { return begin() + i; }
  void LoadEvents(std::vector<Note>* events);
  uint32_t max_tick = 0;

  std::thread* worker;
#pragma endregion
};
}  // namespace midi