#ifndef Tracks_h_
#define Tracks_h_

#include "HardwareInterface.h"
#include <LedControl.h>

enum PlayMode {
  Forward = 0,
  Backward = 1,
  Random = 2
};

enum OutMode {
  Trigger = 0,
  Clock = 1,
  Gate = 2
};

struct Track {
  int length;
  int pattern;
  int position;
  PlayMode play;
  OutMode out;
};

struct Settings {
  Track tracks[3];
  byte version;
};

class Tracks {
public:
  Tracks();
  void updatePattern(int track, int position);
  void applyOffset(int track, int offset);
  void setLength(int track, int offset);
  void setPlayMode(int track, PlayMode mode);
  void setOutMode(int track, OutMode mode);
  int getLength(int track);
  int getPattern(int track);
  int getPosition(int position);
  int getStep(int track);
  PlayMode getPlayMode(int track);
  OutMode getOutMode(int track);
  void stepOn();
  void save();
  void reset();
private:
  bool change = false;
  Settings settings;
  void stepOn(Track* track);
  void load();
  void initialise();
};

#endif
