#include "Display.h"
#include <Arduino.h>

#define FLASH_TIME 250
#define MATRIX_ROWS 8
#define MATRIX_COLUMNS 8
#define ALL_ON 255
#define ALL_OFF 0


Display::Display() {
}

void Display::initialise() {
  matrix.initialise();
  animation();
}

void Display::render() {
  for (int row = 0; row < MATRIX_ROWS; ++ row) {
    if (((nextDisplay[row].state ^ currentDisplay[row].state) != 0) || (currentCursor.active && row == currentCursor.row)) {
      matrix.setRow(row, nextDisplay[row].state);
      currentDisplay[row] = nextDisplay[row];
    }
  }
  updateCursor();
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

void Display::setRows(int row, unsigned int state) {
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

void Display::drawRowIndicator(int row, int position) {
    byte indicator = 0;
    bitSet(indicator, position);
    setRow(row, indicator);
}

void Display::drawRowsIndicator(int row, int position) {
    int indicator = 0;
    bitSet(indicator, position);
    setRows(row, indicator);
}

void Display::drawRowBar(int row, int length) {
    int state = 0;
    fill(state, length);
    setRow(row, lowByte(state));
}

void Display::drawRowsBar(int row, int length) {
    int state = 0;
    fill(state, length);
    setRows(row, state);
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

void Display::drawRowPattern(int row, byte pattern) {
    setRow(row, pattern);
}

void Display::drawRowsPattern(int row, int pattern) {
    drawRowPattern(row, lowByte(pattern));
    drawRowPattern(row + 1, highByte(pattern));
}

void Display::fill(int &value, int length) {
  for(int i = 0; i <= length; ++i) {
    bitSet(value, i);
  }
}

void Display::animation() {
  for (int repeat = 0; repeat < 3; ++ repeat) {
    showInverseSmileyFace();
    delay(200);
    showSmileyFace();
    delay(200);
  }
  showInverseSmileyFace();
  delay(200);
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
