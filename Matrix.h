#ifndef Matrix_h_
#define Matrix_h_

#include "HardwareInterface.h"
#include <LedControl.h>

class Matrix : public HardwareInterface {
public:
  Matrix();
  virtual void initialise();
  void clear();
  void setLed(int row, int column, bool state);
  void setRow(int row, byte state);
private:
  LedControl matrix;
};

#endif
