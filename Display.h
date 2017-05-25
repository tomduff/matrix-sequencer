#ifndef Display_h_
#define Display_h_

#include "HardwareInterface.h"
#include "Matrix.h"
#include <LedControl.h>

struct DisplayRow {
  byte state = 0;
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
  void showCursor();
  void hideCursor();
  void drawRowIndicator(int row, int position);
  void drawRowsIndicator(int row, int position);
  void drawRowBar(int row, int length);
  void drawRowsBar(int row, int length);
  void drawRowCursor(int row, int position);
  void drawRowsCursor(int row, int position);
  void drawRowPattern(int row, byte pattern);
  void drawRowsPattern(int row, int pattern);
  void setLed(int row, int column, bool state);
private:
  bool hasCursorMoved();
  void updateCursor();
  void animation();
  void showSmileyFace();
  void showInverseSmileyFace();
  void fill(int &value, int length);
  //void setLed(int row, int column, bool state);
  void setRow(int row, byte state);
  void setRows(int row, unsigned int state);
  DisplayRow currentDisplay[8];
  DisplayRow nextDisplay[8];
  Cursor currentCursor;
  Cursor nextCursor;
  Matrix matrix = Matrix();
  unsigned long cursorTime = 0;
  bool cursorState = true;
};

#endif
