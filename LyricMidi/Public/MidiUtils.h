#pragma once

#include <cinttypes>
#include <vector>

#include "MidiTypes.h"

using std::vector;

#define MIDI_HEADER_BYTES \
  (uint8_t)0x4D, (uint8_t)0x54, (uint8_t)0x68, (uint8_t)0x64
#define MIDI_TRACK_BYTES \
  (uint8_t)0x4D, (uint8_t)0x54, (uint8_t)0x72, (uint8_t)0x6B

#define VARIADIC_VECTOR(ReturnType, FunctionName, InType)    \
  template <typename... Ts>                                  \
  ReturnType FunctionName(Ts... xs) {                        \
    static_assert((std::is_same<Ts, InType>::value && ...)); \
    return FunctionName({xs...});                            \
  }

#define VARIADIC_VECTOR_PARAM(ReturnType, FunctionName, Param1Type,         \
                              Param1Name, InType, ConstQualifier)           \
  template <typename... Ts>                                                 \
  ReturnType FunctionName(Param1Type Param1Name, Ts... xs) ConstQualifier { \
    static_assert((std::is_same<Ts, InType>::value && ...));                \
    return FunctionName(Param1Name, {xs...});                               \
  }

#define BOLD "\x1b[1m"
#define UNBOLD "\x1b[0m"

#define UPPER_NIBBLE(x) x >> 4
#define LOWER_NIBBLE(x) x & 0x0F

namespace midi {
uint32_t ReadVarLen(Iter* it);
}  // namespace midi