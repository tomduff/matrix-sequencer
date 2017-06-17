#ifndef ClockGenerator_h_
#define ClockGenerator_h_

#include "Io.h"

class ClockGenerator {
public:
  ClockGenerator();
  Signal tick();
  void setSpeed(int offset);
  void setWidth(int offset);
  void setMulitplier(int offset);
  int getSpeed();
  int getWidth();
  int getMulitplier();
  void start();
  void stop();
  void reset();
  bool isRunning();
private:
  unsigned long last;
  long speed;
  int width;
  int multiplier;
  int running;
};

#endif
