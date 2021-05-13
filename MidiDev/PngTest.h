#include "ColorConstants.h"
#include "MidiFile.h"
#include "pngwriter.h"

using namespace std;

void MakePng(midi::File& File) {
  const std::vector<Color> colormap = {COLOR_CONSTANTS};
  pngwriter png(128, File.duration, 0, "out.png");

  for (uint8_t i = 0; i < 128; i++) {
    for (size_t t = 0; t < File.duration; t++) {
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
          continue;
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
      // we are adding 1 to the positions because this fn takes 1-based
      // coordinates...
      png.plot(i + 1, t + 1, col->r / color_divisor, col->g / color_divisor,
               col->b / color_divisor);
    }
  }
}