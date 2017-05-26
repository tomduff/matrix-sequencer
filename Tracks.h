#ifndef Tracks_h_
#define Tracks_h_

#include "HardwareInterface.h"
#include <LedControl.h>

enum TrackMode {
  Forward,
  Backward,
  Random
};

struct Track {
  int length;
  int pattern;
  int position;
  TrackMode mode;
};

class Tracks {
public:
  Tracks();
  void updatePattern(int track, int position);
  void applyOffset(int track, int offset);
  void setLength(int track, int offset);
  int getLength(int track);
  int getPattern(int track);
  int getPosition(int position);
  int getStep(int track);
  void stepOn();
private:
  Track tracks[3];
  void stepOn(Track* track);
};

#endif
