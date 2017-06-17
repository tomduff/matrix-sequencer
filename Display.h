#ifndef Display_h_
#define Display_h_

#include "HardwareInterface.h"
#include "Matrix.h"
#include "Tracks.h"
#include <LedControl.h>
#include <stdint.h>

const uint64_t MODES[] PROGMEM = {
  0x0001000000001818,   // Pattern Mode
  0x0002001818001818,   // Track Mode
  0x0004001818006666,   // Modifiers Mode
  0x0008006666006666,   // Clock Mode
  0x001000666600dbdb,   // Reset Mode
  0x002000dbdb00dbdb    // System Mode
};

const uint64_t PLAY_MODES[] PROGMEM = {
  0x0010307fff7f3010,   // Forward
  0x00080cfefffe0c08,   // Backwards
  0x0081c3e7ffe7c381,   // Forwards Backwards
  0x0066361e3e66663e    // R (Random)
};

const uint64_t OUT_MODES[] PROGMEM = {
  0x00ff020202020202,   // Trigger
  0x00db4a4a4a4a4a6e,   // Clock Variable
  0x00c342424242427e    // Gate
};

const uint64_t DIVIDER_TYPES[] PROGMEM = {
  0x00060e0c08683818,   // Beat
  0x031bdad292929ce0    // Triplet
};

const uint64_t BEAT_DIVIDERS[] PROGMEM {
  0x0000000008000000,   // Divider 1
  0x0000000018000000,   // Divider 2
  0x0000001818000000,   // Divider 4
  0x0000181818180000,   // Divider 8
  0x00003c3c3c3c0000,   // Divider 16
  0x3c3c3c3c3c3c3c3c,   // Divider 32
  0xffffffffffffffff    // Divider 64
};

const uint64_t TRIPLET_DIVIDERS[] PROGMEM {
  0x000000001c000000,   // Divider 3
  0x0000001c1c000000,   // Divider 6
  0x0000001c1c1c0000,   // Divider 9
  0x00001c1c1c1c0000,   // Divider 12
  0x001c1c1c1c1c0000,   // Divider 15
  0x001c1c1c1c1c1c00,   // Divider 18
  0x1c1c1c1c1c1c1c00,   // Divider 21
  0x1c1c1c1c1c1c1c1c    // Divider 24
};

const uint64_t MUTATION_SEEDS[] PROGMEM = {
  0x003c66666666663c,   // O (Original)
  0x003c66060606663c,   // C (Current)
  0x003c18181818183c    // I (Inverse Current)
};

const uint64_t PATTERN_MODES[] PROGMEM = {
  0x0006063e6666663e,   // P (Programmed)
  0x007e06063e06067e    // E (Euclidean)
};

const uint64_t SHUFFLE[] PROGMEM = {
  0x0000000000000000,
  0x0000000000000001,
  0x0000000000000101,
  0x0000000000000301,
  0x0000000000020301,
  0x0000000000060301,
  0x0000000004060301,
  0x000000000c060301,
  0x000000080c060301,
  0x000000180c060301,
  0x000010180c060301,
  0x000030180c060301,
  0x002030180c060301,
  0x006030180c060301,
  0x406030180c060301,
  0xc06030180c060301
};

const uint64_t MUTATION_CHANCE[] PROGMEM = {
  0x0000000000000000,
  0x0100000000000000,
  0x0300000000000000,
  0x0302000000000000,
  0x0702000000000000,
  0x0706000000000000,
  0x0706040000000000,
  0x0f06040000000000,
  0x0f0e040000000000,
  0x0f0e0c0000000000,
  0x0f0e0c0800000000,
  0x1f0e0c0800000000,
  0x1f1e0c0800000000,
  0x1f1e1c0800000000,
  0x1f1e1c1800000000,
  0x1f1e1c1810000000,
  0x3f1e1c1810000000,
  0x3f3e1c1810000000,
  0x3f3e3c1810000000,
  0x3f3e3c3810000000,
  0x3f3e3c3830000000,
  0x3f3e3c3830200000,
  0x7f3e3c3830200000,
  0x7f7e3c3830200000,
  0x7f7e7c3830200000,
  0x7f7e7c7830200000,
  0x7f7e7c7870200000,
  0x7f7e7c7870600000,
  0x7f7e7c7870604000,
  0xff7e7c7870604000,
  0xfffe7c7870604000,
  0xfffefc7870604000,
  0xfffefcf870604000,
  0xfffefcf8f0604000,
  0xfffefcf8f0e04000,
  0xfffefcf8f0e0c000,
  0xfffefcf8f0e0c080,
  0xfffefcf8f0e0c080
};

const uint64_t CLOCK_WIDTH[] PROGMEM = {
  0x00ff020202020200,
  0x00ff020202020600,
  0x00ff020202060600,
  0x00ff020206060600,
  0x00ff020606060600,
  0x00ff060606060600,
  0x00ff060606060e00,
  0x00ff0606060a0e00,
  0x00ff06060a0a0e00,
  0x00ff060a0a0a0e00,
  0x00ff0a0a0a0a0e00,
  0x00fb0a0a0a0a0e00,
  0x00fb0a0a0a0a1e00,
  0x00fb0a0a0a121e00,
  0x00fb0a0a12121e00,
  0x00fb0a1212121e00,
  0x00fb121212121e00,
  0x00f3121212121e00,
  0x00f3121212123e00,
  0x00f3121212223e00,
  0x00f3121222223e00,
  0x00f3122222223e00,
  0x00f3222222223e00,
  0x00e3222222223e00,
  0x00e3222222223e00,
  0x00e3222222227e00,
  0x00e3222222427e00,
  0x00e3222242427e00,
  0x00e3224242427e00,
  0x00e3424242427e00,
  0x00c3424242427e00
};

const uint64_t CLOCK_STATE[] PROGMEM = {
  0x00003c3c3c3c0000,
  0x000c1c3c3c1c0c00
};

const uint64_t OFFBEAT_OUTPUT[] PROGMEM = {
  0x3c4281998989423c,
  0xffe7e7e7e7e7e7ff
};

const uint64_t SMILE PROGMEM = 0x003c420024242400;
const uint64_t INVERSE_SMILE PROGMEM = 0xffc3bdffdbdbdbff;

struct DisplayFrame {
  uint64_t image;
  unsigned long time;
  unsigned long start;
  bool active;
  bool clocked;
};

struct DisplayRow {
  byte state = 0;
};

struct Indicator {
  int row;
  int column;
  bool active;
  unsigned long start;
};

struct TrackIndicator {
  int track = 0;
  bool active = false;
  unsigned long start = 0;
};

struct Cursor {
  int row = 0;
  int position = 0;
  bool active = false;
};

class Display : public HardwareInterface {
public:
  Display();
  virtual void initialise();
  void clear();
  void timeout();
  void render();
  void drawProgrammedView(int track, int pattern);
  void drawEuclideanView(int track, int pattern);
  void drawOffsetView(int track, int pattern);
  void drawLengthView(int track, int length);
  void drawShuffleView(int track, int length);
  void drawLengthView(int track, int start, int end, bool active);
  void drawPlayModeView(int track, PlayMode mode);
  void drawOutModeView(int track, OutMode mode);
  void drawPlayView(int track, int position, int pattern, bool cursor);
  void drawDividerView(int track, int divider, DividerType type);
  void drawDividerTypeView(int track, DividerType type);
  void drawPatternTypeView(int track, PatternType mode);
  void drawMutationView(int track, int mutation);
  void drawMutationSeedView(int track, MutationSeed seed);
  void drawClockSpeed(bool state);
  void drawClockWidth(int width);
  void drawOffbeatOutput(bool offBeat);
  void setTrackCursor(int track, int position);
  void indicateReset();
  void indicateClock();
  void indicateTrack(int track);
  void indicateActiveTrack(int track);
  void indicateMode(int mode);
private:
  void updateFlashState();
  void showCursor(int track, bool visible);
  void clear(DisplayRow rows[]);
  void showIndicator(Indicator& indicator);
  void updateIndicators();
  void updateIndicator(Indicator& indicator);
  void updateTrackIndicator(TrackIndicator& indicator);
  bool hasCursorMoved();
  void updateCursors();
  void updateFrame();
  int getFrameRow(int row);
  void updateCursorMask(int cursor);
  void simley();
  void showSmileyFace();
  void showInverseSmileyFace();
  void fill(int &value, int length, int bit);
  void setRange(int &value, int start, int end, int bit);
  void setRow(int row, byte state);
  void setRows(int row, int state);
  void setLed(int row, int column, bool state);
  void showFrame(const uint64_t *image);
  void showClockedFrame(const uint64_t *image);
  void showTimedFrame(const uint64_t *image, unsigned long time);
  void showFrame(const uint64_t *image, unsigned long time, bool clocked);
  DisplayRow display[8];
  DisplayRow cursorMask[8];
  DisplayFrame frame;
  Cursor cursors[3];
  Indicator clock;
  Indicator reset;
  Indicator tracks[4];
  TrackIndicator activeTrack;
  Matrix matrix = Matrix();
  unsigned long cursorTime = 0;
  bool flashState = true;
};

#endif
