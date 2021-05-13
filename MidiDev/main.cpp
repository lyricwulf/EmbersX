#include <windows.h>
#include <wingdi.h>

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "ColorConstants.h"
#include "MidiFile.h"
#include "PixelBuffer.h"
#include "portable-file-dialogs.h"

#include "VulkanizedEngine.hpp"

constexpr auto BASE_PATH = "D:/Projects/LyricMidi/black";

// for debugging
struct TimePair {
	std::string name;
	std::chrono::steady_clock::time_point time;
};

int main(int argc, char* argv[]) {
	// Set console stuff
	SetConsoleTitle("Lyric Midi Player");
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	CONSOLE_FONT_INFOEX fontInfo;
	fontInfo.cbSize = sizeof(fontInfo);
	fontInfo.FontFamily = 54;
	fontInfo.FontWeight = 400;
	fontInfo.nFont = 0;
	// const wchar_t myFont[] = L"Lucida Sans Unicode";
	fontInfo.dwFontSize = { 18, 20 };
	// std::copy(myFont, myFont + (sizeof(myFont) / sizeof(wchar_t)),
	// fontInfo.FaceName);

	SetCurrentConsoleFontEx(hConsole, false, &fontInfo);

	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = 0;  // set the cursor visibility
	SetConsoleCursorInfo(hConsole, &cursorInfo);

	std::string MidiPath = "0";

	if (argc == 2) {
		MidiPath = argv[1];
	}

	while (!std::filesystem::exists(MidiPath))
	{
		std::cout << "Opening file dialog" << std::endl;
		auto selection = pfd::open_file("Select a MIDI file", ".",
			{ "MIDI Files", "*.mid *.midi",
			  "All Files", "*" }).result();
		if (!selection.empty())
			MidiPath = selection[0];
	}

	// Load midi as pixel buffer

	std::vector<TimePair> times;
	std::chrono::steady_clock::time_point last_time =
		std::chrono::high_resolution_clock::now();
	times.push_back({ "Started   ", std::chrono::high_resolution_clock::now() });

	std::string BasePath = BASE_PATH;
	auto File = midi::File(MidiPath.c_str());

	// this is mostly buffer stuff, I think :s
	times.push_back({ "File made ", std::chrono::high_resolution_clock::now() });

	File.Ready();

	times.push_back({ "File ready", std::chrono::high_resolution_clock::now() });

	// File.DebugOutput(std::cout);

	// TODO: Optimize memory of maps by storing a delta tick for consecutive vals

	// prints!!
	uint8_t* byte_buffer = FileToBuffer(File);
	size_t size = sizeof(uint8_t) * 128 * File.duration;

	times.push_back({ "Buffer made", std::chrono::high_resolution_clock::now() });

	for (auto&& time : times) {
		std::cout << "\x1b[1m" << time.name << " in "
			<< (time.time - last_time).count() / 1000000. << " ms"
			<< std::endl;
		last_time = time.time;
	}

	std::cout << "\x1b[1mFor a file of size: "
		<< std::filesystem::file_size((MidiPath).c_str()) / 1000.
		<< " kB" << std::endl;

	float ticks_per_second = File.ppqn * 120 / 60.0;

	if (!File[128]->empty())
	{
		ticks_per_second = File.ppqn * (*File[128])[0] / 60.0;
	}

	VulkanizedEngine app(byte_buffer, size, ticks_per_second);

	try {
		app.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	cout << "Exiting." << endl;
	return EXIT_SUCCESS;

	return 0;
}
