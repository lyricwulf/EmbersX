#pragma region[Includes]

#include "MidiBuffer.h"

#pragma endregion

namespace midi {

#pragma region[Public]

uint32_t ntohl(uint32_t in) { return _byteswap_ulong(in); }
uint16_t ntohl(uint16_t in) { return _byteswap_ushort(in); }

bool Buffer::Has(Iter Start, Bytes Values) const {
  // Make sure the Buffer is long enough to hold the Values
  if ((end() - Start) < Values.size()) return false;

  // Make sure the values match the input Values
  for (size_t i = 0; i < Values.size(); ++i)
    if (*(Start + i) != Values[i]) return false;

  // Passed all checks!
  return true;
}

Iter Buffer::Find(Bytes Values) {
  auto It = this->begin();
  while (true) {
    // Find the first value
    It = std::find(It, this->end(), Values[0]);

    // Check if there was no value found
    // Return because this is the end of the buffer search
    if (It == this->end()) return this->begin();  //-1;

    // Make sure the remaining Buffer is long enough to hold all reminaing
    // Values
    if (this->end() - It < Values.size()) continue;

    // Make sure the subsequent values match the input Values
    for (size_t i = 1; i < Values.size(); ++i)
      if (Values[i] != *(It + i)) goto Next;

    // Add the index that was found
    return It;  // FoundIndex;
  Next:;
    It++;
  }
}

Iters Buffer::FindAll(Bytes Values) {
  Iters Ret;
  auto It = this->begin();
  while (true) {
    // Find the first value
    It = std::find(It, this->end(), Values[0]);

    // Check if there was no value found
    // Return because this is the end of the buffer search
    if (It == this->end()) return Ret;

    // Make sure the remaining Buffer is long enough to hold all reminaing
    // Values
    if (this->end() - It < Values.size()) continue;

    // Make sure the subsequent values match the input Values
    for (size_t i = 1; i < Values.size(); ++i)
      if (Values[i] != *(It + i)) goto Next;

    // Add the index that was found
    Ret.push_back(It);

  Next:;
    It++;
  }
}

uint16_t Buffer::AsInt16(size_t i) const {
  return Buffer::AsInt16(begin() + i);
}
uint32_t Buffer::AsInt32(size_t i) const {
  return Buffer::AsInt32(begin() + i);
}

uint16_t Buffer::AsInt16(Iter StartingAt) {
  return *(StartingAt + 0) << 8 |  //
         *(StartingAt + 1) << 0;   //
}
uint32_t Buffer::AsInt32(Iter StartingAt) {
  return *(StartingAt + 0) << 24 |  //
         *(StartingAt + 1) << 16 |  //
         *(StartingAt + 2) << 8 |   //
         *(StartingAt + 3) << 0;    //
}

#pragma endregion

}  // namespace midi