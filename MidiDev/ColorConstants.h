#pragma once

#define COLOR_CONSTANTS                                                        \
  {255, 0, 0}, {255, 96, 0}, {255, 191, 0}, {223, 255, 0}, {128, 255, 0},      \
      {32, 255, 0}, {0, 255, 64}, {0, 255, 159}, {0, 255, 255}, {0, 159, 255}, \
      {0, 64, 255}, {32, 0, 255}, {128, 0, 255}, {223, 0, 255}, {255, 0, 191}, \
  {                                                                            \
    255, 0, 96                                                                 \
  }

struct alignas(32) Color {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  uint8_t a = 0;
  uint8_t operator[](uint8_t i) const {
    switch (i) {
      case 0:
        return r;
        break;
      case 1:
        return g;
        break;
      case 2:
        return b;
        break;
      default:
        return 0;
    }
  }
};