#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

const uint WIDTH = 128;
const float WHITE_WIDTH = 74.65;
const float WHITE_OFFSET = 0.05;
float WHITE = fragTexCoord.x * WHITE_WIDTH + WHITE_OFFSET;
const float edge_threshhold = 0.05;
const float note_edge_power = 4;
#define PACKAGE_BYTES 4

// float px_wide ();

// max width
layout(constant_id = 0) const uint HEIGHT = 16777216;  // default: 2^24

const vec3[16] ColorLUT = {
    {0.729, 0, 0.051},     {0.769, 0.11, 0},      {0.776, 0.412, 0},
    {0.78, 0.569, 0},      {0.784, 0.725, 0},     {0.6, 0.667, 0},
    {0.353, 0.573, 0.086}, {0.031, 0.498, 0.137}, {0, 0.404, 0.357},
    {0, 0.545, 0.639},     {0, 0.478, 0.757},     {0, 0.412, 0.753},
    {0, 0.161, 0.518},     {0.196, 0.043, 0.525}, {0.416, 0, 0.502},
    {0.69, 0, 0.227}};

layout(std430, set = 0, binding = 0) readonly buffer SsboBuffer {
  readonly uint data[HEIGHT][WIDTH / PACKAGE_BYTES];
}
midiBuffer;

layout(set = 0, binding = 1) readonly uniform UboBuffer {
  readonly float world_tick;
  readonly float speed;
  readonly vec2 window_size;
}
State;

float tick = (State.world_tick - (State.speed * fragTexCoord.y));

uint key = uint(fragTexCoord.x * WIDTH);  // 0 - 127 for which note
float key_progress = fract(fragTexCoord.x * WIDTH);
bool edge = key_progress < edge_threshhold;
bool white_edge = fract(WHITE) < edge_threshhold;
// fract(mod(fragTexCoord.x, 12 / 128.0) * WHITE_WIDTH) < edge_threshhold;

// 1.0 / State.window_size.y);
float piano_height =
    1.0 - (State.window_size.x * 6.0 / WHITE_WIDTH) / State.window_size.y;

bool is_in_piano_zone = fragTexCoord.y > piano_height;
float piano_white_height = piano_height + (1 - piano_height) * 0.65;
bool is_in_white_zone = fragTexCoord.y > piano_white_height;
bool key_is_white = fract(key * 5 / 12.0) < 0.1 || fract(key * 5 / 12.0) > 0.55;

#define DARKEN_FRONT(x) (x & 0x10) > 0
#define DARKEN_BACK(x) (x & 0x20) > 0
#define BOOST_FRONT(x) (x & 0x40) > 0

float edge_pow(in float dist);
float edge_pow(in float dist, in float power);
uint get_data(in uint tick, in uint index);

void main() {
  if (is_in_piano_zone) {
    int piano_tick = int(State.world_tick - (State.speed * piano_height));

    if (key_is_white || is_in_white_zone) {
      // base white color
      outColor = vec4(0.8);

      // use white key to determine the lookup index
      uint key_from_white = uint(uint(WHITE) * 12 / 7.0 + 0.75);

      // use a fixed height for the tick we are looking at

      // Get data from LUT if data is not blank
      uint data = get_data(uint(piano_tick), key_from_white);
      if ((data & 0xF0) > 0 && piano_tick >= 0) {
        outColor = vec4(ColorLUT[data & 0x0F], 1);
      }

      // darken edges!
      outColor = outColor  // base
                 * edge_pow(fract(WHITE), 32) *
                 edge_pow(1 - fract(WHITE), 32)  // x
                 * edge_pow((fragTexCoord.y - piano_height), 256);
    } else {
      // key is white or black
      outColor = vec4(0.2);
      uint data = get_data(uint(piano_tick), key);

      if ((data & 0xF0) > 0 && piano_tick >= 0) {
        outColor = vec4(ColorLUT[data & 0x0F], 1);
      }

      outColor = outColor * edge_pow(fract(fragTexCoord.x * 128), 8) *
                 edge_pow(1 - fract(fragTexCoord.x * 128), 8);
    }

    return;
  }

  float h_dist_from_center = 2 * abs(key_progress - 0.5);
  // the background.
  vec4 bg_color = vec4(0.15, 0.15, 0.15, 1.0)  // base color
                  * (1 - (0.5 - fragTexCoord.x) * (0.5 - fragTexCoord.x))  // x
                  * fragTexCoord.y;                                        // y

  /*
    if (edge) {
      outColor = bg_color + vec4(0.1, 0.1, 0.1, 0);
    }*/

  // This expands the packed data
  uint data = get_data(uint(tick), key);

  // ignore background by most sig bit
  // also ignore if tick is out of bounds
  if (((data & 0x80) == 0x00) || tick < 0.0) {
    outColor = bg_color;
    return;
  }

  // get channel from data
  uint ch = uint(data & 0x0F);
  vec3 color = ColorLUT[ch];

  // apply modifiers
  float coefficient = 1;
  // shading!
  coefficient *= (1 - key_progress);
  // soft outline on notes :)
  coefficient *= edge_pow(1 - h_dist_from_center);
  // darkens
  coefficient *= DARKEN_FRONT(data) ? 0.5 + fract(tick) / 2 : 1;
  coefficient *= DARKEN_BACK(data) ? 0.5 + (1 - fract(tick)) / 2 : 1;
  coefficient *= BOOST_FRONT(data) ? 1 + (1 - fract(tick)) : 1;

  outColor = vec4(color * coefficient, 1.0);
}

float edge_pow(in float dist, in float power) {
  return (1 - pow(1 - dist, power));
}
float edge_pow(in float dist) { return edge_pow(dist, note_edge_power); }

uint get_data(in uint tick, in uint index) {
  uint base = index / (PACKAGE_BYTES);
  uint shift = 8 * (index % PACKAGE_BYTES);

  return (midiBuffer.data[tick][base] >> shift) & 0xFF;
}

/*
  return;

  uint data_prev = (midiBuffer.data[tick_index - 1][base] >> shift) & 0xFF;
  uint ch_prev = uint(data_prev & 0x0F);
  uint data_next = (midiBuffer.data[tick_index + 1][base] >> shift) & 0xFF;
  uint ch_next = uint(data_next & 0x0F);

  // highlight front if this note is on
  float boost1 = (data & 0x80) > 0 ? 1 - fract(tick) : 0;
  float boost = 1 + boost1;

  // darken back if next note is on
  float darken1 = (data_next & 0x80) > 0 ? 1 - fract(tick) : 1;
  // darken front if prev note is off
  float darken2 = (data_prev & 0x20) > 0 ? fract(tick) : 1;
  // darken back if this note is off
  float darken3 = (data & 0x20) > 0 ? edge_pow(1 - fract(tick)) : 1;

  // darken only when there is no boost applied
  float darken = boost > 1 ? 1 : darken1 * darken2 * darken3;

  outColor =
      vec4(color * darken * boost * edge_pow(1 - h_dist_from_center), 1.0);
  return;

  // THIS IS SOME COOL STUFF WITH NOTE STARTS AND ENDS BUT SOMETIMES IT DOESN'T
  // WORK TOO WELL?? let's skip it for now

  // data parsing
  switch (data & 0xF0) {
    // note on
    case 0x80:
      float dist =
          1 - sqrt(pow(1 - fract(tick), 2) + pow((key_progress - 0.5) * 2, 2));
      if (dist < 0) {
        // this pixel is out of bounds
        uint data_prev =
            (midiBuffer.data[tick_index - 1][base] >> shift) & 0xFF;
        uint ch_prev = uint(data_prev & 0x0F);
        if ((data_prev & 0xF0) == 0) {
          // last tick was empty, paint background
          outColor = bg_color;
          return;
        }
        // last tick had a color so paint that
        vec3 color_prev = ColorLUT[ch_prev] * shading_color_mod *
                          edge_pow(1 - h_dist_from_center);
        outColor = vec4(color_prev, 1.0);
        return;
      }
      // paint start of note
      outColor = vec4(color * edge_pow(dist), 1.0);
      return;

    // note body
    case 0x40:
      outColor = vec4(color * edge_pow(1 - h_dist_from_center), 1.0);
      return;

    // note off
    case 0x20:
      float dist_o =
          1 - sqrt(pow(fract(tick), 2) + pow((key_progress - 0.5) * 2, 2));
      if (dist_o < 0) {
        // this pixel is out of bounds
        uint data_next =
            (midiBuffer.data[tick_index + 1][base] >> shift) & 0xFF;
        uint ch_next = uint(data_next & 0x0F);
        if ((data_next & 0xF0) == 0) {
          // next tick is empty, paint background
          outColor = bg_color;
          return;
        }
        // next tick has a color so paint that
        vec3 color_next = ColorLUT[ch_next] * shading_color_mod *
                          edge_pow(1 - h_dist_from_center);
        outColor = vec4(color_next, 1.0);
        return;
      }
      // paint end of note
      outColor = vec4(color * edge_pow(dist_o), 1.0);
      return;

      // nothing. draw background
    case 0x00:
      outColor = bg_color;
      break;
      // invalid upper nibble
      outColor = vec4(1, 0, 1, 1);
  }
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