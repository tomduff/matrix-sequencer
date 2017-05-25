#include "Matrix.h"

#define MATRIX_ADDRESS 0

Matrix::Matrix()
  : matrix(2, 3, 4, 1) {
}

void Matrix::initialise() {
  matrix.setIntensity(MATRIX_ADDRESS, 0);
  matrix.shutdown(MATRIX_ADDRESS, true);
  matrix.clearDisplay(MATRIX_ADDRESS);
  matrix.shutdown(MATRIX_ADDRESS, false);
}

void Matrix::setLed(int row, int column, bool state) {
  matrix.setLed(MATRIX_ADDRESS, row, column, state);
}

void Matrix::setRow(int row, byte state) {
  matrix.setRow(MATRIX_ADDRESS, row, state);
}

void Matrix::clear() {
  matrix.clearDisplay(MATRIX_ADDRESS);
}
