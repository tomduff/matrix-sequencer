#ifndef Tracks_h_
#define Tracks_h_

#include "HardwareInterface.h"
#include <LedControl.h>

enum PlayMode {
  Forward = 0,
  Backward = 1,
  Random = 2,
  Pendulum = 3
};

enum OutMode {
  Trigger = 0,
  Clock = 1,
  Gate = 2
};

enum TrackType {
  Pattern,
  Euclidean
};

enum ClockDivider {
  OneBeat = 1,
  TwoBeat = 2,
  ThreeBeat = 3,
  FourBeat = 4,
  EightBeat = 8,
  SixteenBeat = 16,
  ThirtyTwoBeat = 32,
  SixtyFourBeat = 64,
  OneTriplet = 3,
  TwoTriplet = 6,
  ThreeTriplet = 9,
  FourTriplet = 12,
  FiveTriplet = 15,
  SixTriplet = 18,
  SevenTriplet = 21,
  EightTriplet = 24
};

struct Track {
  int length;
  int pattern;
  TrackType type;
  PlayMode play;
  OutMode out;
  ClockDivider divider;
};

struct TrackState {
  int position;
  PlayMode direction;
  int beat;
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
  void nextPlayMode(int track);
  void setOutMode(int track, OutMode mode);
  void nextOutMode(int track);
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
  Track tracks[3];
  TrackState state[3];
  void stepOn(int track);
  void load();
  void initialise(Track& track);
  void initialise(TrackState& track);
};

#endif
