#pragma once

#include <cinttypes>
#include <vector>

namespace midi {
typedef uint8_t Byte;
typedef typename std::vector<Byte> Bytes;
typedef typename Bytes::const_iterator Iter;
typedef typename std::vector<Iter> Iters;
typedef typename Bytes::iterator IterW;
typedef typename std::vector<IterW> IterWs;

struct Note {
  // Note(uint32_t tick, uint8_t channel, uint8_t key, uint8_t velocity);
  uint32_t tick;
  uint8_t channel;
  uint8_t key;
  uint8_t velocity;

  uint32_t AsTempo() { return channel << 16 | key << 8 | velocity; }
  friend bool operator<(const Note& l, const Note& r) {
    return std::tie(l.tick, l.channel) <
           std::tie(r.tick, r.channel);  // keep the same order
  }
};
}  // namespace midi