#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform ConstantBlock {
  readonly float world_tick;
  readonly float speed;
}
PushConstant;

const uint WIDTH = 128;
const float edge_threshhold = 0.05;
#define PACKAGE_BYTES 4

layout(constant_id = 0) const uint HEIGHT = 16777216;  // default: 2^24

const vec3[16] testlut = {
    {255, 0, 0},   {255, 96, 0},  {255, 191, 0}, {223, 255, 0},
    {128, 255, 0}, {32, 255, 0},  {0, 255, 64},  {0, 255, 159},
    {0, 255, 255}, {0, 159, 255}, {0, 64, 255},  {32, 0, 255},
    {128, 0, 255}, {223, 0, 255}, {255, 0, 191}, {255, 0, 96}};

layout(std430, set = 0, binding = 0) readonly buffer MidiBuffer {
  uint data[HEIGHT][WIDTH / PACKAGE_BYTES];
}
midiBuffer;

void main() {
  uint key = uint(fragTexCoord.x * WIDTH);  // 0 - 127 for which note
  float progress = fract(fragTexCoord.x * WIDTH);

  // -- KEY 0 --  -- KEY 1 --  -- KEY 2 --  -- KEY 3 --
  // base = 0;    base = 0;    base = 0;    base = 0;
  // shift = 3;   shift = 2;   shift = 1;   shift = 0;
  uint base = key / (PACKAGE_BYTES);
  uint shift = 8 * (key % PACKAGE_BYTES);
  // keep.endian: 8 * (((128 + 3) - key) % 4);
  // swch.endian: 8 * (key % 4);

  bool edge = fract((fragTexCoord.x * WIDTH)) < edge_threshhold;
  // + (edge_threshhold / 2)) < edge_threshhold;

  // This expands the packed data
  uint tick_index = world_tick * fragTexCoord.y;
  uint data = (midiBuffer.data[tick_index][base] >> shift) & 0xFF;

  // get channel from data
  uint ch = uint(data & 0x0F);

  // data parsing
  bool on = (data & 0x80) > 0;
  bool mid = (data & 0x60) > 0;
  bool off = (data & 0x10) > 0;

  float outR = data / 256.0;
  float outG = float(edge) * 0.2;
  float outB = float(edge) * 0.2;

  outColor = vec4(testlut[ch] / 255.0, 1.0);  // vec4(outR, outG, outB, 1.0);
  // vec4(outX, outY, outZ, 1.f);  // vec4(outX, outY, outZ, 1.0);
}

/*
 * Data parsing notes... :1
 *
 * -- CHUNK 1 --
 * [0][0] 0b00000000 -> 0x00 |
 * [0][1] 0b00000001 -> 0x01 |
 * [0][2] 0b00000010 -> 0x02 |
 * [0][3] 0b00000011 -> 0x03 |-> 0x00010203
 *
 * -- CHUNK 2 --
 * [0][4] 0b00000100 -> 0x04 |
 * [0][5] 0b00000101 -> 0x05 |
 * [0][6] 0b00000110 -> 0x06 |
 * [0][7] 0b00000111 -> 0x07 |-> 0x04050607
 *
 * -- IN --
 * [0][0] -> 0x00010203
 * [0][1] -> 0x04050607
 *
 * ----PARSING ----
 * base = key / 4;
 * shift = (131 - key) % 4;
 *
 * -- KEY 0 --  -- KEY 1 --  -- KEY 2 --  -- KEY 3 --
 * base = 0;    base = 0;    base = 0;    base = 0;
 * shift = 3;   shift = 2;   shift = 1;   shift = 0;
 *
 *
 *
 */