#include "Display.h"
#include <Arduino.h>

#define row(t) (t * 2)

#define INDICATOR_TIME 25
#define INDICATOR_ROW 7
#define CLOCK_INDICATOR 0
#define RESET_INDICATOR 1
#define TRACK_ONE_INDICATOR 2
#define TRACK_TWO_INDICATOR 5
#define TRACK_THREE_INDICATOR 7
#define OFF_BEAT_INDICATOR 3
#define FLASH_TIME 250
#define MATRIX_ROWS 8
#define MATRIX_COLUMNS 8
#define ALL_ON 255
#define LED_ON 1
#define ALL_OFF 0
#define LED_OFF 0


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

  updateIndicators();

  for (int row = 0; row < MATRIX_ROWS; ++ row) {
    if (((nextDisplay[row].state ^ currentDisplay[row].state) != 0) || (currentCursor.active && row == currentCursor.row)) {
      matrix.setRow(row, nextDisplay[row].state);
      currentDisplay[row] = nextDisplay[row];
    }
  }
  updateCursor();
}

void Display::updateIndicators() {
  updateIndicator(clock);
  updateIndicator(reset);
  for(int track = 0; track <= 3; ++track) {
    updateIndicator(tracks[track]);
  }
}

void Display::updateCursor() {

    if(nextCursor.active) {
      unsigned long now = millis();
      if(!currentCursor.active || (currentCursor.row != nextCursor.row) || (currentCursor.position != nextCursor.position)) {
        cursorTime = now;
        cursorState = !bitRead(currentDisplay[nextCursor.row].state, nextCursor.position);
      } else if (cursorTime != 0 && (now - cursorTime > FLASH_TIME)) {
        cursorTime = now;
        cursorState = !cursorState;
      }
      matrix.setLed(nextCursor.row, nextCursor.position, cursorState);
    } else {
      cursorTime = 0;
    }
    currentCursor = nextCursor;
}

void Display::showCursor() {
  nextCursor.active = true;
}

void Display::hideCursor() {
  nextCursor.active = false;
}

void Display::drawPatternView(int track, int pattern) {
    setRows(row(track), pattern);
}

void Display::drawLengthView(int track, int length) {
    int state = 0;
    fill(state, length);
    setRows(row(track), state);
}

void Display::drawPlayModeView(int track, PlayMode mode) {
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
    }
    setRow(row(track) + 1, ALL_OFF);
}

void Display::drawOutModeView(int track, OutMode mode) {
    switch(mode) {
      case OutMode::Trigger:
        setRow(row(track), B00000010);
        setRow(row(track) + 1, B00000101);
        break;
      case OutMode::Clock:
        setRow(row(track), B00000110);
        setRow(row(track) + 1, B00001001);
      break;
      case OutMode::Gate:
        setRow(row(track), B00011110);
        setRow(row(track) + 1, B00100001);
      break;
    }
}

void Display::drawPlayView(int track, int position, int pattern) {
  int state = 0;
  if(position < MATRIX_COLUMNS) {
    state = lowByte(pattern);
    position += MATRIX_COLUMNS;
  } else {
    state = highByte(pattern);
  }
  bitSet(state, position);
  setRows(row(track), state);
}

void Display::setRows(int row, int state) {
  setRow(row, lowByte(state));
  setRow(row + 1, highByte(state));
}

void Display::setLed(int row, int column, bool state) {
  bitWrite(nextDisplay[row].state, column, state);
}

void Display::setRow(int row, byte state) {
  nextDisplay[row].state = state;
}

void Display::clear() {
  for (int row = 0; row < MATRIX_ROWS; ++row) {
    nextDisplay[row].state = ALL_OFF;
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

void Display::showIndicator(Indicator& indicator) {
    indicator.active = true;
    indicator.start = millis();
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

void Display::drawRowCursor(int row, int position) {
  nextCursor.row = row;
  nextCursor.position = MATRIX_COLUMNS - position - 1;
}

void Display::drawRowsCursor(int row, int position) {
  if(position >= MATRIX_ROWS) {
    ++row;
    position %= MATRIX_ROWS;
  }
  drawRowCursor(row, position);
}

void Display::fill(int &value, int length) {
  for(int i = 0; i <= length; ++i) {
    bitSet(value, i);
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
