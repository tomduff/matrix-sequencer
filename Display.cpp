#include "Display.h"
#include <Arduino.h>

#define row(t) (t * 2)

#define INDICATOR_TIME 25
#define TRACK_INDICATOR_TIME 500
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
  : clock{INDICATOR_ROW, CLOCK_INDICATOR,false,0},
    reset{INDICATOR_ROW, RESET_INDICATOR,false,0},
    tracks{{INDICATOR_ROW,TRACK_ONE_INDICATOR,false,0},{INDICATOR_ROW,TRACK_TWO_INDICATOR,false,0},{INDICATOR_ROW,TRACK_THREE_INDICATOR,false,0},{INDICATOR_ROW,OFF_BEAT_INDICATOR,false,0}} {
}

void Display::initialise() {
  matrix.initialise();
  simley();
}

void Display::render() {
  updateCursors();
  updateIndicators();
  for (int row = 0; row < MATRIX_ROWS; ++ row) matrix.setRow(row, display[row].state ^ cursorMask[row].state);
}

void Display::updateIndicators() {
  updateIndicator(clock);
  updateIndicator(reset);
  for(int track = 0; track <= TRACKS; ++track) updateIndicator(tracks[track]);
  updateTrackIndicator(activeTrack);
}

void Display::updateCursors() {
  clear(cursorMask);
  unsigned long now = millis();
  if ((cursorState && (now - cursorTime) > FLASH_TIME_ON) || (!cursorState && (now - cursorTime) > FLASH_TIME_OFF)) {
    cursorState = !cursorState;
    cursorTime = now;
  }
  for (int cursor = 0; cursor < CURSORS; ++cursor) updateCursorMask(cursor);
}

void Display::updateCursorMask(int cursor) {
    if(cursors[cursor].active) {
      bitWrite(cursorMask[cursors[cursor].row].state, cursors[cursor].position, cursorState);
    }
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
    cursorMask[row].state = cursorState ? ALL_ON : ALL_OFF;
    cursorMask[row + 1].state = cursorState ? ALL_ON : ALL_OFF;
    if (indicator.start == 0 || (millis() - indicator.start > TRACK_INDICATOR_TIME)) {
      indicator.active = false;
      indicator.start = 0;
    }
  }
}

void Display::showCursor(int track, bool visible) {
  cursors[track].active = visible;
}

void Display::drawProgrammedView(int track, int pattern) {
  showCursor(track, true);
  setRows(row(track), pattern);
}

void Display::drawEuclideanView(int track, int pattern) {
  showCursor(track, false);
  setRows(row(track), pattern);
}

void Display::drawOffsetView(int track, int pattern) {
  showCursor(track, false);
  setRows(row(track), pattern);
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

void Display::drawLengthView(int track, int start, int end, bool active) {
  setTrackCursor(track, active ? end : start);
  showCursor(track, true);
  int state = 0;
  setRange(state, start, end, LED_ON);
  setRows(row(track), state);
}

void Display::drawLengthView(int track, int length) {
  drawLengthView(track, 0, length, true);
}

void Display::drawPlayModeView(int track, PlayMode mode) {
  showCursor(track, false);
  switch(mode) {
    case PlayMode::Forward:
      setRow(row(track), B00000111);
      break;
    case PlayMode::Backward:
      setRow(row(track), B11100000);
    break;
    case PlayMode::Random:
      setRow(row(track), B10100010);
    break;
    case PlayMode::Pendulum:
      setRow(row(track), B11100111);
    break;
    break;
  }
  setRow(row(track) + 1, ALL_OFF);
}

void Display::drawOutModeView(int track, OutMode mode) {
  showCursor(track, false);
  switch(mode) {
    case OutMode::Trigger:
      setRow(row(track), B00000010);
      setRow(row(track) + 1, B00000101);
      break;
    case OutMode::Clock:
      setRow(row(track), B00001110);
      setRow(row(track) + 1, B00010001);
    break;
    case OutMode::Gate:
      setRow(row(track), B01111110);
      setRow(row(track) + 1, B10000001);
    break;
  }
}

void Display::drawPatternTypeView(int track, PatternType mode) {
  showCursor(track, false);
  switch(mode) {
    case PatternType::Programmed:
      setRow(row(track), ALL_ON);
      setRow(row(track) + 1, ALL_OFF);
      break;
    case PatternType::Euclidean:
      setRow(row(track), ALL_OFF);
      setRow(row(track) + 1, ALL_ON);
    break;
  }
}

void Display::drawDividerView(int track, int divider, DividerType type) {
  showCursor(track, false);
  byte state = 0;
  bitSet(state, divider);
  setRow(row(track), state);
  switch(type) {
    case Beat:
      setRow(row(track) + 1, B00000001);
      break;
    case Triplet:
      setRow(row(track) + 1, B00000111);
      break;
  }
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

void Display::indicateMode(int mode) {
  byte state = 0;
  bitSet(state, mode);
  setRow(MODE_ROW, state);
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
    showInverseSmileyFace();
    delay(100);
    showSmileyFace();
    delay(100);
  }
  showInverseSmileyFace();
  delay(100);
  clear();
  render();
}

void Display::showSmileyFace() {
  for (int row = 0; row < MATRIX_ROWS; ++ row) {
    if (row == 0 || row == 4 || row == 7) setRow(row, ALL_OFF);
    if (row == 1 || row == 2 || row == 3) setRow(row, B00100100);
    if (row == 5) setRow(row, B01000010);
    if (row == 6) setRow(row, B00111100);
  }
  render();
}

void Display::showInverseSmileyFace() {
  for (int row = 0; row < MATRIX_ROWS; ++ row) {
    if (row == 0 || row == 4 || row == 7) setRow(row, ALL_ON);
    if (row == 1 || row == 2 || row == 3) setRow(row, B11011011);
    if (row == 5) setRow(row, B10111101);
    if (row == 6) setRow(row, B11000011);
  }
  render();
}
