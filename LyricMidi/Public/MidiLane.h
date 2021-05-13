#pragma once

#include <algorithm>
#include <iterator>
#include <vector>

#include "MidiEvent.h"

namespace midi {

class Lane : public std::vector<Note> {
#pragma region[Managers]
 public:
  Lane() : std::vector<Note>(){};
  Lane(size_t size) : std::vector<Note>() { reserve(size); };
#pragma endregion

#pragma region[Public]
 public:
  void DebugOutput(std::ostream& Out);

  Lane& operator+=(const Lane* y) {
    if (y->size() < 1) return *this;

    size_t len = this->size();

    if (len < 1) {
      this->reserve(y->size());
      this->insert(this->begin(), y->begin(), y->end());
      return *this;
    };

    this->reserve(len + y->size());

    this->insert(this->end(), y->begin(), y->end());
    std::inplace_merge(this->begin(), this->begin() + len, this->end());

    return *this;
  }
#pragma endregion

#pragma region[Private]
 private:
  /*
   friend Lane operator+(Lane lhs, const Lane& rhs) {
     lhs.reserve(lhs.size() + rhs.size());
     Lane::iterator middle = lhs.end();
     lhs.insert(lhs.end(), rhs.begin(), rhs.end());
     std::inplace_merge(lhs.begin(), middle, lhs.end());
     return lhs;
   }

   friend Lane operator+(Lane lhs, const Lane* rhs) {
     if (lhs.size() < 1) return *rhs;
     if (rhs->size() < 1) return lhs;
     lhs.reserve(lhs.size() + rhs->size());
     Lane::iterator middle = lhs.end();
     lhs.insert(lhs.end(), rhs->begin(), rhs->end());
     std::inplace_merge(lhs.begin(), middle, lhs.end());
     return lhs;
   }
 */
#pragma endregion
};
}  // namespace midi