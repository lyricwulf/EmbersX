#include "MidiTrack.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <thread>

#include "MidiBuffer.h"
#include "MidiUtils.h"

using std::endl;
using std::make_shared;
using std::thread;

namespace midi {
#pragma region[Managers]

	// Construct with filestream
	Track::Track(uint32_t number, istream& fs, uint32_t in_length) {
		Number = number;
		length = in_length;
		buffer.resize(length);
		fs.read(reinterpret_cast<char*>(buffer.data()), length);
		thread = new std::thread(&Track::ParseEvents, this);
		channel_addend = number % 16;
	}

	// Construct Track (deprecated)
	Track::Track(uint32_t number, Iter head, uint32_t length) {
		// Set(number, head, length);
	}

#pragma endregion
#pragma region[Public]

	void Track::DebugOutput(std::ostream& Out) {
		Out << BOLD "TRACK " << Number << UNBOLD << endl;
		Out << "-- Byte length: " << length << endl;
		Out << "-- Tick duration: " << Duration << endl;
		uint64_t size = 0;
		for (Lane lane : lanes) {
			size += lane.size();
		}
		Out << "-- Events: " << size << endl;
	}

	void Track::Ready() const {
		if (thread->joinable()) thread->join();
	}

#pragma endregion

#pragma region[Private]

	// Parses events into event buffer
	bool Track::ParseEvents() {
		// Skip advancing head because we already are on first event
		// Iter current_byte = Head + 8;
		for (Lane lane : lanes) {
			lane.reserve(length / 100);
		}
		Iter current_byte = buffer.begin();
		uint32_t absolute_time = 0;  // ?This may need to be extended to uint64_t

		// 08-EF case
		// MIDI Event
		// <v_time> + <midi_event> | <meta_event> | <sysex_event>

		// F0-FE case
		// SysEx Event
		// <v_time>

		// FF
		// Meta Event
		// <v_time> + 0xFF + <meta_type> + <v_length> + <event_data_bytes>

		uint8_t type;

		while (current_byte < buffer.end()) {
			// 1. DELTA TIME -------------------------------------------------------
			// Adapted from http://midi.teragonaudio.com/tech/midifile/vari.htm
			// We need the byte length of dt so we should not remove this function
			// (Alternatively, this may be macro'd if necessary)
			// * NOTE: CURRENT BYTE IS DELTA TIME --> [DT (DT) (DT)] EV D1 (D2)
			// uint8_t dt_byte_length = 0;
			uint32_t delta_time = 0;
			uint8_t b;

			do {
				b = *(current_byte++);
				delta_time = (delta_time << 7) | (b & 0b01111111);
			} while (b & 0b10000000);

			// 2. EVENT TYPE --------------------------------------------------
			// We are ignoring EVERYTHING except Note On and Note Off for now.
			// * NOTE: CURRENT BYTE IS EVENT TYPE --> dt (dt) (dt) [EV] D1 (D2)
			absolute_time += delta_time;

			// check if repeated event!
			if (*(current_byte) >= 0x80) type = *(current_byte++);

			// * NOTE: CURRENT BYTE IS DATA1 --> dt (dt) (dt) EV [D1] (D2) ...

			// Ignore anything above NoteOff ch 16
			if (type > 0x9F) {
				if (type < 0xC0) {  // Up to Controller
					current_byte += 2;
					continue;
				}
				if (type < 0xE0) {  // Up to Channel Pressure
					current_byte += 1;
					continue;
				}
				if (type < 0xF0) {  // Pitch bend
					current_byte += 2;
					continue;
				}
				// * Meta events (0xFF) have an extra byte before len
				if (type == 0xFF) {
					// Check if tempo event... 0x51
					if (*(current_byte++) == 0x51) {
						// Check the length, should be 3
						_ASSERT(*(current_byte++) == 0x03);
						lanes[128].push_back({ absolute_time, *(current_byte++),
											  *(current_byte++), *(current_byte++) });
						continue;
					};
				}

				current_byte += *(current_byte++);
				// current_byte += meta_length;

				continue;
			}

			// Event was NoteOn or NoteOff
			// NOTE: CURRENT BYTE IS DATA 1 --> dt (dt) (dt) EV [D1] (D2)
			uint8_t channel = channel_addend;//((type & 0x0F) + channel_addend) % 16;
			uint8_t data1 = *(current_byte++);  // key number
			uint8_t data2 = *(current_byte++);
			bool note_on = (type >= 0x90) && (data2 > 0);
			lanes[data1].push_back(
				{ absolute_time, channel, data1, static_cast<uint8_t>(note_on * data2) });
		}

		Duration = absolute_time;

		return true;
	}

#pragma endregion
}  // namespace midi