#include "MidiLane.h"

#include <ostream>

#include "MidiUtils.h"

namespace midi {
void Lane::DebugOutput(std::ostream& Out) {
  Out << BOLD << "LANE X, " << size() << " events" << std::endl;
}
}  // namespace midi