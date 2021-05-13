#pragma region[Includes]

#include "MidiFile.h"

#include <cassert>
#include <fstream>
#include <future>
#include <memory>
#include <thread>

#include "MidiMap.h"
#include "MidiUtils.h"

#pragma endregion

using midi::Buffer;
using midi::File;
using std::endl;
using std::ifstream;
using std::streamsize;
using std::unique_ptr;

namespace midi {

#pragma region[Managers]

	// Default constructor
	File::File(const char* Path) {
		ifstream fs(Path, std::ios::binary | std::ios::ate);

		// Stop if file is invalid for any reason
		if (!fs.good()) throw 400;

		fs.seekg(0, std::ios::beg);

		// We happen to know the length of the header
		buffer = Buffer(8);

		// Read up to and including the length
		//  [M T h d 00 00 00 06]
		fs.read(reinterpret_cast<char*>(buffer.data()), 8);
		// Verify header bytes exist
		if (!Good()) throw 0x4D546864;  // MThd

		// Read length. We can overwrite the old data buffer as it's no longer needed.
		uint32_t header_length = buffer.AsInt32(4);  // "should" be 6
		fs.read(reinterpret_cast<char*>(buffer.data()), header_length);
		// Read the rest of the header
		// next bytes are given as 2<format>, 2<n>, 2<division>
		format = static_cast<FileFormat>(buffer.AsInt16(0));
		track_count = buffer.AsInt16(2);
		ppqn = buffer.AsInt16(4);

		// buffer is now ready to be parsed
		ParseTracks(fs);
	}

	// Default destructor
	File::~File() {}

#pragma endregion

#pragma region[Public]

	// Maybe add more checks later
	bool File::Good() const { return HasValidHeader(); }

	void File::Ready() {
		for (Track* track : tracks) {
			track->Ready();
			duration = std::max(duration, track->Duration);
		}

		for (size_t i = 0; i < 129; ++i) {
			master_lanes[i] = new Lane();
			for (Track* track : tracks) {
				*master_lanes[i] += (*track)[i];
			}
		}

		for (int i = 0; i < 129; ++i) {
			master_maps[i] = new Map(duration, master_lanes[i]);
		}

		for (int i = 0; i < 129; ++i) {
			master_maps[i]->Ready();
		}
	}

	void File::DebugOutput(std::ostream& Out) const {
		// Total buffer size
		size_t sum = 8 + 6 + (8 * track_count);
		for (Track* t : tracks) {
			sum += t->Size();
			t->DebugOutput(Out);
		}
		Out << BOLD << "Total buffer size: " << sum / 1024 << " kB" << endl;

		// Note event size
		sum = 0;
		for (int i = 0; i < 128; ++i) {
			// debug maps
			master_maps[i]->DebugOutput(Out);
			// size up the lanes
			sum += master_lanes[i]->size();
			if (master_lanes[i]->empty()) continue;
		}

		Out << BOLD "HEADER" UNBOLD << endl;
		Out << "-- Format: " << format << endl;
		Out << "-- Tracks: " << track_count << endl;
		Out << "-- PPQN:   " << ppqn << endl;
		Out << BOLD << "Note events: " << sum << endl;
		Out << BOLD << "Tempo events: " << master_lanes[128]->size() << endl;
		Out << BOLD << "Duration: " << duration << " ticks" << endl;
		Out << BOLD << "Approx. map cache size: "
			<< (duration * sizeof(uint8_t) * 128 / 1000000) << " MB ~ "
			<< (duration * sizeof(uint32_t) * 128 / 1000000) << " MB" << endl;
	}

#pragma endregion

#pragma region[Private]

	// Header attributes
	// header_chunk = "MThd" + <header_length> + <format> + <n> + <division>
	//              0x 0123     4567              89         AB    CD

	bool File::HasValidHeader() const {
		return buffer.Has(buffer.begin(), MIDI_HEADER_BYTES);
	}

	Iter File::HeaderHead() const { return buffer.begin(); }

	// Tracks
	void File::ParseTracks(std::istream& fs) {
		tracks.reserve(track_count);
		for (uint32_t i = 0; i < track_count; ++i) {
			fs.read(reinterpret_cast<char*>(buffer.data()), 8);
			assert(buffer.Has(buffer.begin(), MIDI_TRACK_BYTES));
			uint32_t track_length = buffer.AsInt32(4);
			Track* track = new Track(i, fs, track_length);
			tracks.push_back(track);
			// current_track_head += DispatchTrack(i, current_track_head);
		}
	}

	uint32_t File::DispatchTrack(uint32_t track_number, Iter head) {
		/*
		uint32_t length = buffer.AsInt32(head + 4);
		Track *track = new Track();
		tracks.push_back(track);
		std::thread *worker =
			new std::thread([=] { track->Set(track_number, head, length); });
		// worker->detach();
		threads.push_back(worker);
		return length + 8;
		*/
		return 0;
	}

#pragma endregion

}  // namespace midi
