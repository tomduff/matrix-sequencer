#ifndef Tracks_h_
#define Tracks_h_

#include "HardwareInterface.h"
#include <LedControl.h>

struct Track {
  int length;
  int pattern;
};

class Tracks {
public:
  Tracks();
  void updatePattern(int track, int position);
  void updateLength(int track, int offset);
  void offset(int track, int offset);
  int length(int track);
  int pattern(int track);
private:
  Track tracks[3];
};

#endif
