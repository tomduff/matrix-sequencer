#include "Display.h"
#include <Arduino.h>

#define row(t) (t * 2)

#define UNLIMITED 0
#define INDICATOR_TIME 25
#define TRACK_INDICATOR_TIME 300
#define MODE_FRAME_TIME 500
#define FLASH_TIME_ON 50
#define FLASH_TIME_OFF 100
#define INDICATOR_ROW 7
#define MODE_ROW 6
#define CLOCK_INDICATOR 0
#define RESET_INDICATOR 1
#define TRACK_ONE_INDICATOR 2
#define TRACK_TWO_INDICATOR 5
#define TRACK_THREE_INDICATOR 7
#define OFF_BEAT_INDICATOR 3
#define MATRIX_ROWS 8
#define MATRIX_COLUMNS 8
#define ALL_ON 255
#define LED_ON 1
#define ALL_OFF 0
#define LED_OFF 0
#define TRACKS 3
#define CURSORS TRACKS

Display::Display()
  : frame{0, 0, 0, false},
    clock{INDICATOR_ROW, CLOCK_INDICATOR, false, 0},
    reset{INDICATOR_ROW, RESET_INDICATOR, false, 0},
    tracks{{INDICATOR_ROW, TRACK_ONE_INDICATOR, false, 0},
      {INDICATOR_ROW, TRACK_TWO_INDICATOR, false, 0},
      {INDICATOR_ROW, TRACK_THREE_INDICATOR, false, 0},
      {INDICATOR_ROW, OFF_BEAT_INDICATOR, false, 0}} {
}

void Display::initialise() {
  matrix.initialise();
  simley();
}

void Display::render() {
  updateFlashState();
  updateFrame();
  updateCursors();
  updateIndicators();

  for (int row = 0; row < MATRIX_ROWS; ++ row) {
    if (frame.active) matrix.setRow(row, getFrameRow(row));
    else matrix.setRow(row, display[row].state ^ cursorMask[row].state);
  }
}

int Display::getFrameRow(int row) {
  int frameRow = (frame.image >> row * MATRIX_ROWS) & 0xFF;
  if (frame.clocked && !clock.active) frameRow = frameRow & frame.image;
  return frameRow;
}

void Display::updateFlashState() {
  unsigned long now = millis();
  if ((flashState && (now - cursorTime) > FLASH_TIME_ON) || (!flashState && (now - cursorTime) > FLASH_TIME_OFF)) {
    flashState = !flashState;
    cursorTime = now;
  }
}

void Display::updateFrame() {
  if (frame.active) {
    if (frame.time != 0 && (millis() - frame.start > frame.time)) {
      frame.active = false;
      frame.start = 0;
    }
  }
}

void Display::updateCursors() {
  clear(cursorMask);
  for (int cursor = 0; cursor < CURSORS; ++cursor) updateCursorMask(cursor);
}

void Display::updateCursorMask(int cursor) {
    if(cursors[cursor].active) {
      bitWrite(cursorMask[cursors[cursor].row].state, cursors[cursor].position, flashState);
    }
}

void Display::updateIndicators() {
  updateIndicator(clock);
  updateIndicator(reset);
  for(int track = 0; track <= TRACKS; ++track) updateIndicator(tracks[track]);
  updateTrackIndicator(activeTrack);
}

void Display::updateIndicator(Indicator& indicator) {
  if (indicator.active) {
    if (indicator.start == 0 || (millis() - indicator.start > INDICATOR_TIME)) {
      indicator.active = false;
      indicator.start = 0;
    }
    setLed(indicator.row, indicator.column, indicator.active);
  }
}

void Display::updateTrackIndicator(TrackIndicator& indicator) {
  if (indicator.active) {
    int row = row(indicator.track);
    setRows(row, ALL_OFF);
    cursorMask[row].state = flashState ? ALL_ON : ALL_OFF;
    cursorMask[row + 1].state = flashState ? ALL_ON : ALL_OFF;
    if (indicator.start == 0 || (millis() - indicator.start > TRACK_INDICATOR_TIME)) {
      indicator.active = false;
      indicator.start = 0;
    }
  }
}

void Display::showCursor(int track, bool visible) {
  cursors[track].active = visible;
}

void Display::drawPlayView(int track, int position, int pattern, bool cursor) {
  showCursor(track, cursor);
  setRows(row(track), pattern);
  setTrackCursor(track, position);
  // int state = 0;
  // if(position < MATRIX_COLUMNS) {
  //   state = lowByte(pattern);
  //   position += MATRIX_COLUMNS;
  // } else {
  //   state = highByte(pattern);
  // }
  // bitSet(state, position);
  // setRows(row(track), state);
}

void Display::drawProgrammedView(int track, int pattern) {
  timeout();
  showCursor(track, true);
  setRows(row(track), pattern);
}

void Display::drawEuclideanView(int track, int pattern) {
  timeout();
  showCursor(track, false);
  setRows(row(track), pattern);
}

void Display::drawOffsetView(int track, int pattern) {
  timeout();
  showCursor(track, false);
  setRows(row(track), pattern);
}

void Display::drawLengthView(int track, int start, int end, bool active) {
  timeout();
  setTrackCursor(track, active ? end : start);
  showCursor(track, true);
  int state = 0;
  setRange(state, start, end, LED_ON);
  setRows(row(track), state);
}

void Display::drawLengthView(int track, int length) {
  timeout();
  drawLengthView(track, 0, length, true);
}

void Display::drawShuffleView(int track, int shuffle) {
  showFrame(&SHUFFLE[shuffle]);
}

void Display::drawPlayModeView(int track, PlayMode mode) {
  showFrame(&PLAY_MODES[mode]);
}

void Display::drawOutModeView(int track, OutMode mode) {
  showFrame(&OUT_MODES[mode]);
}

void Display::drawPatternTypeView(int track, PatternType mode) {
  showFrame(&PATTERN_MODES[mode]);
}

void Display::drawDividerView(int track, int divider, DividerType type) {
  switch(type) {
    case Beat:
      showFrame(&BEAT_DIVIDERS[divider]);
      break;
    case Triplet:
      showFrame(&TRIPLET_DIVIDERS[divider]);
      break;
  }
}

void Display::drawDividerTypeView(int track, DividerType type) {
  showFrame(&DIVIDER_TYPES[type]);
}

void Display::drawMutationView(int track, int mutation) {
  showFrame(&MUTATION_CHANCE[mutation]);
}

void Display::drawMutationSeedView(int track, MutationSeed seed) {
  showFrame(&MUTATION_SEEDS[seed]);
}

void Display::drawClockSpeed(bool state) {
  if(state) showClockedFrame(&CLOCK_STATE[state]);
  else showFrame(&CLOCK_STATE[state]);
}

void Display::drawClockWidth(int width) {
  showFrame(&CLOCK_WIDTH[width]);
}

void Display::drawOffbeatOutput(bool offBeat) {
  showFrame(&OFFBEAT_OUTPUT[offBeat]);
}

void Display::setRows(int row, int state) {
  setRow(row, lowByte(state));
  setRow(row + 1, highByte(state));
}

void Display::setLed(int row, int column, bool state) {
  bitWrite(display[row].state, column, state);
}

void Display::setRow(int row, byte state) {
  display[row].state = state;
}

void Display::clear() {
  clear(display);
  timeout();
}

void Display::timeout() {
  frame.active = false;
}

void Display::clear(DisplayRow rows[]) {
  for (int row = 0; row < MATRIX_ROWS; ++row) {
    rows[row].state = ALL_OFF;
  }
}

void Display::indicateClock() {
  showIndicator(clock);
}

void Display::indicateReset() {
  showIndicator(reset);
}

void Display::indicateTrack(int track) {
  showIndicator(tracks[track]);
}

void Display::indicateActiveTrack(int track) {
  activeTrack.track = track;
  activeTrack.active = true;
  activeTrack.start = millis();
}

void Display::showIndicator(Indicator& indicator) {
    indicator.active = true;
    indicator.start = millis();
}

void Display::showFrame(const uint64_t *image) {
  showFrame(image, UNLIMITED, false);
}

void Display::showClockedFrame(const uint64_t *image) {
  showFrame(image, UNLIMITED, true);
}

void Display::showTimedFrame(const uint64_t *image, unsigned long time) {
  showFrame(image, time, false);
}

void Display::showFrame(const uint64_t *image, unsigned long time, bool clocked) {
  memcpy_P(&frame.image, image, MATRIX_ROWS);
  frame.time = time;
  frame.start = millis();
  frame.active = true;
  frame.clocked = clocked;
}

void Display::indicateMode(int mode) {
  byte state = 0;
  bitSet(state, mode);
  setRow(MODE_ROW, state);
  showTimedFrame(&MODES[mode], MODE_FRAME_TIME);
}

void Display::setTrackCursor(int track, int position) {
  int row = row(track);
  if(position >= MATRIX_ROWS) {
    ++row;
    position %= MATRIX_ROWS;
  }
  cursors[track].row = row;
  cursors[track].position = position;
}

void Display::fill(int &value, int length, int bit) {
  setRange(value, 0, length, bit);
}

void Display::setRange(int &value, int start, int end, int bit) {
  for(int i = start; i <= end; ++i) {
    bitWrite(value, i, bit);
  }
}

void Display::simley() {
  for (int repeat = 0; repeat < 3; ++ repeat) {
    showFrame(&INVERSE_SMILE);
    render();
    delay(100);
    showFrame(&SMILE);
    render();
    delay(100);
  }
  showFrame(&INVERSE_SMILE);
  render();
  delay(100);
  clear();
  render();
}
