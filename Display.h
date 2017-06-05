#ifndef Display_h_
#define Display_h_

#include "HardwareInterface.h"
#include "Matrix.h"
#include "Tracks.h"
#include <LedControl.h>

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
  void render();
  void drawProgrammedView(int track, int pattern);
  void drawEuclideanView(int track, int pattern);
  void drawOffsetView(int track, int pattern);
  void drawLengthView(int track, int length);
  void drawLengthView(int track, int start, int end, bool active);
  void drawPlayModeView(int track, PlayMode mode);
  void drawOutModeView(int track, OutMode mode);
  void drawPlayView(int track, int position, int pattern, bool cursor);
  void drawDividerView(int track, int divider, DividerType type);
  void drawPatternTypeView(int track, PatternType mode);
  void setTrackCursor(int track, int position);
  void indicateReset();
  void indicateClock();
  void indicateTrack(int track);
  void indicateActiveTrack(int track);
  void indicateMode(int mode);
private:
  void showCursor(int track, bool visible);
  void clear(DisplayRow rows[]);
  void showIndicator(Indicator& indicator);
  void updateIndicators();
  void updateIndicator(Indicator& indicator);
  void updateTrackIndicator(TrackIndicator& indicator);
  bool hasCursorMoved();
  void updateCursors();
  void updateCursorMask(int cursor);
  void simley();
  void showSmileyFace();
  void showInverseSmileyFace();
  void fill(int &value, int length, int bit);
  void setRange(int &value, int start, int end, int bit);
  void setRow(int row, byte state);
  void setRows(int row, int state);
  void setLed(int row, int column, bool state);
  DisplayRow display[8];
  DisplayRow cursorMask[8];
  Cursor cursors[3];
  Indicator clock;
  Indicator reset;
  Indicator tracks[4];
  TrackIndicator activeTrack;
  Matrix matrix = Matrix();
  unsigned long cursorTime = 0;
  bool cursorState = true;
};

#endif
