#include <execution>

#include "ColorConstants.h"
#include "MidiFile.h"

using namespace std;

auto pup = std::execution::par_unseq;

/*
void* FileToColorBuffer(midi::File& File) {
	const std::vector<Color> colormap = { COLOR_CONSTANTS };

	uint8_t* bitmap_buffer =
		static_cast<uint8_t*>(malloc(sizeof(uint8_t) * 128 * File.duration * 4));

	int index[128];  // vector with 128 ints.
	std::iota(std::begin(index), std::end(index), 0);

	std::vector<int> time(File.duration);  // vector with [duration] ints.
	std::iota(std::begin(time), std::end(time), 0);

	for_each(pup, std::begin(index), std::end(index), [=](const int& i) {
		for_each(pup, std::begin(time), std::end(time), [=](const int& t) {
			uint8_t ch = (*File[i])[t];
			uint8_t channel = ch & 0x0F;
			uint8_t mod = ch & 0xF0;

			// see which type of data is here
			uint8_t color_divisor;
			switch (mod) {
				// check if pixel is blank
			case 0b00000000:
				// pixel is black by default so we can skip the set. !change
				// this if we want to change bg color
				return;  // continue;
			case 0b10000000:
				color_divisor = 1;
				break;
			case 0b01000000:
				color_divisor = 2;
				break;
			case 0b00100000:
				color_divisor = 4;
				break;
			default:
				throw;
			}
			const Color* col = &colormap[channel];
			for (int j = 0; j < 3; j++) {
				bitmap_buffer[(i + (t * 128)) * 4 + j] = (*col)[j];
			}
			});
		});

	return bitmap_buffer;
}
*/

uint8_t* FileToBuffer(midi::File& File) {
	uint8_t* byte_buffer =
		static_cast<uint8_t*>(malloc(sizeof(uint8_t) * 128 * File.duration));

	int index[128];  // vector with 128 ints.
	std::iota(std::begin(index), std::end(index), 0);


	for_each(pup, std::begin(index), std::end(index), [=](const int& i) {
		for (int t = 0; t < File.duration; ++t) {
			byte_buffer[(t * 128) + i] = (*File[i])[t];
		}
		});


	return byte_buffer;
}