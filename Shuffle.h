#ifndef Shuffle_h_
#define Shuffle_h_

#include "Shuffle.h"
#include "Io.h"
#include <Encoder.h>

#include "Matrix.h"

struct ShuffleState {
  unsigned long lastClock;
  Signal clockSignal;
  Signal shuffleSignal[4];
  bool newCycle[4];
};

class Shuffle {
public:
  Shuffle();
  void clock(Signal signal);
  Signal tick(int track, int shuffle);
  void reset();
private:
  unsigned long gate;
  int beat;
  ShuffleState state;
  Signal shuffleClock(int track, int shuffle);
  void newCycle();
  Matrix matrix = Matrix();
};

#endif
