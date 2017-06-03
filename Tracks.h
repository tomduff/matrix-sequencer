#ifndef Tracks_h_
#define Tracks_h_

#include "HardwareInterface.h"
#include <LedControl.h>

enum PlayMode {
  Forward = 0,
  Backward = 1,
  Pendulum = 2,
  Random = 3
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

enum DividerType {
  Beat,
  Triplet
};

struct Track {
  int start;
  int end;
  int pattern;
  int density;
  TrackType type;
  PlayMode play;
  OutMode out;
  int divider;
  DividerType dividerType;
};

struct TrackState {
  int pattern;
  int length;
  int position;
  bool forward;
  int beat;
  int division;
  int euclidean;
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
  void setStart(int track, int offset);
  void setEnd(int track, int offset);
  void setPlayMode(int track, int offset);
  void setOutMode(int track, int offset);
  void setDivider(int track, int offset);
  void nextDividerType(int track);
  int getStart(int track);
  int getEnd(int track);
  int getLength(int track);
  int getPattern(int track);
  int getPosition(int position);
  int getDivider(int track);
  int getStep(int track);
  DividerType getDividerType(int track);
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
  void stepPosition(int track);
  void load();
  void initialiseTrack(int track);
  void initialiseState(int track);
  void resetLength(int track);
  void resetPattern(int track);
  void resetDivision(int track);
  int calculateDivision(int divider, DividerType type);
};

#endif
