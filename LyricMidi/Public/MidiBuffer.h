#pragma once

#pragma region[Includes]

#include <cinttypes>
#include <vector>

#include "MidiTypes.h"
#include "MidiUtils.h"

#pragma endregion

namespace midi {

class Buffer : public Bytes {
#pragma region[Managers]
 public:
  Buffer() : Bytes(){};
  Buffer(size_t Count) : Bytes(Count){};

#pragma endregion

#pragma region[Public]
  bool Has(Iter Start, Bytes Values) const;
  VARIADIC_VECTOR_PARAM(bool, Has, Iter, Start, Byte, const);

  Iter Find(Bytes Values);
  VARIADIC_VECTOR(Iter, Find, Byte);

  // Iters FindAll(Byte Value);
  Iters FindAll(Bytes Values);
  VARIADIC_VECTOR(Iters, FindAll, Byte);

  uint16_t AsInt16(size_t i) const;
  uint32_t AsInt32(size_t i) const;
  static uint16_t AsInt16(Iter StartingAt);
  static uint32_t AsInt32(Iter StartingAt);

  const Iter operator[](size_t i) const { return begin() + i; };

#pragma endregion

#pragma region[Private]

 private:
  const Bytes::const_iterator get(size_t i) const { return begin() + i; }
#pragma endregion
};
}  // namespace midi