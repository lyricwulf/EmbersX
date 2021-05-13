#include "MidiMap.h"

#include "ostream"

#define RESET(x) x & 0x0F

namespace midi {

	/*
	 *
	 * Last 4 bytes: channel 0-15
	 * 0b....1010
	 *
	 * Encoding flags:
	 * 0b1....... note body
	 * 0b...1.... darken front
	 * 0b..1..... darken back
	 * 0b.1...... boost front
	 *
	 * "Special" flag 0x80 denotes a trivial look-up operation
	 * 0b0....... (We use 0 because background should be 0000 by default as that is the initialization value)
	 *
	 * Special flags:
	 * 0b.000.... background
	 * (we can add some more sometime idk)
	 *
	 */

	constexpr uint8_t LW_NOTE = 0b1000 << 4;
	constexpr uint8_t LW_DARKEN_FRONT = 0b1001 << 4;
	constexpr uint8_t LW_DARKEN_BACK = 0b1010 << 4;
	constexpr uint8_t LW_BOOST_FRONT = 0b1100 << 4;
	constexpr uint8_t LW_SPECIAL_BG = 0b0000 << 4;

#pragma region[Public]

	void Map::DebugOutput(std::ostream& Out) const {
		Out << "Map " << std::endl;
		Out << "-- Size: " << size() << std::endl;
	}

	void Map::Ready() {
		if (worker->joinable()) worker->join();
	}

#pragma endregion

#pragma region[Private]

	void Map::LoadEvents(std::vector<Note>* events) {
		if (events->size() == 0) return;

		std::forward_list<uint8_t> note_list;
		std::vector<uint32_t> channel_last_ticks(16);
		uint32_t latest_tick_needs_shading = 0;

		Note* next = &(*(events->begin()));
		Note* event = next - 1;
		Note* end = event + events->size();

		for (size_t i = 0; i < events->size(); i++) {
			event++;
			next++;

			const uint8_t ch = event->channel;
			auto tick = event->tick;  // auto in case we change width of tick

			const bool note_on = event->velocity > 0;

			// special to note on
			const bool should_cast_shadow = note_list.empty() ? false : true;
			// special to note off
			const bool off_was_top = note_list.empty() ? false : ch == note_list.front();

			if (note_on) {  // note on
			  // Add note to the top of the list. Store the It in the ptr array
				note_list.push_front(ch);

			}
			else {  // note off
				// Remove the It from the ptr array
				note_list.remove(ch);
			}

			// both cases

			const uint8_t top_channel = note_list.empty()
				? 0  // 0x0000XXXX is background
				: note_list.front();  // set continuous flag


			auto len = (next <= end) ? (next->tick - tick) : 0;
			uint8_t* start_pos = &(*(begin() + tick));
			uint8_t fill_value = note_list.empty() ? LW_SPECIAL_BG : LW_NOTE | top_channel;

			// bulk fill
			uint8_t previous_byte_here = *start_pos;
			memset(start_pos, fill_value, len);

			if (tick == latest_tick_needs_shading && !note_list.empty()) {
				memset(start_pos, fill_value | LW_DARKEN_FRONT, 1);
			}

			// additional start and end flags to set
			if (note_on) {
				// mark as note on! this should override other flags!
				memset(start_pos, ch | LW_BOOST_FRONT, 1);

				uint8_t* prev = start_pos - 1;
				if (*prev & 0x80) {
					memset(prev, *prev | LW_DARKEN_BACK, 1);
				}
			}
			else {
				// only do something if it's visible (aka if the channel was on top)
				if (off_was_top) {
					latest_tick_needs_shading = tick + 1;
					// true to bypass this because it doesn't work yet
					// it is supposed to detect a self-terminating note but this logic not good
					if (channel_last_ticks[ch] != tick) {
						// normal case!
						uint8_t prev = *(start_pos - 1);
						memset(start_pos - 1, prev | LW_DARKEN_BACK, 1);
					}
					else {
						// self-terminating case!
						memset(start_pos, ch | LW_BOOST_FRONT | LW_DARKEN_BACK, 1);
					}
				}
				// mark as note off!
			}

			channel_last_ticks[ch] = tick;
		}

		// mark the rest of the buffer as empty
		// memset(&(*(begin() + last_tick)), 0, max_tick - last_tick);

		// we should be able to deallocate this...
		// events->clear();
		// events->shrink_to_fit();
	}

#pragma endregion

}  // namespace midi

#undef RESET