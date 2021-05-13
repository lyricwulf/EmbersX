#include <iostream>
#include <cinttypes>
#include <vector>

#include "VulkanizedEngine.hpp"

using namespace std;
int mainExample() {
	constexpr size_t width = 128;
	size_t height = 1000;

	uint8_t* midi_buffer = static_cast<uint8_t*>(malloc(height * width * sizeof(uint8_t)));

	for (size_t y = 0; y < height; ++y) {
		for (size_t x = 0; x < width; ++x) {
			auto idx = y * width + x;
			midi_buffer[idx] = x % 16;
			//memset(midi_buffer + idx, x % 16, 1);
		}
	}

	VulkanizedEngine app(midi_buffer, width * height, 192);

	try {
		app.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	cout << "Exiting." << endl;
	return EXIT_SUCCESS;
}