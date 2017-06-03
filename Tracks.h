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

enum PatternType {
  Programmed,
  Euclidean
};

enum DividerType {
  Beat,
  Triplet
};

struct Track {
  int pattern;
  int start;
  int end;
  int length;
  int density;
  int offset;
  int divider;
  PlayMode play;
  OutMode out;
  PatternType patternType;
  DividerType dividerType;
};

struct TrackState {
  int pattern;
  int length;
  int position;
  bool forward;
  int beat;
  int division;
};

struct Settings {
  Track tracks[3];
  byte version;
};

class Tracks {
public:
  Tracks();
  void updatePattern(int track, int position);
  void rotatePattern(int track, int offset);
  void setOffset(int track, int offset);
  void setDensity(int track, int offset);
  void setLength(int track, int offset);
  void setStart(int track, int offset);
  void setEnd(int track, int offset);
  void setPlayMode(int track, int offset);
  void setOutMode(int track, int offset);
  void setDivider(int track, int offset);
  void nextPatternType(int track);
  void nextDividerType(int track);
  int getStart(int track);
  int getEnd(int track);
  int getLength(int track);
  int getPattern(int track);
  int getPosition(int position);
  int getDivider(int track);
  int getStep(int track);
  PatternType getPatternType(int track);
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
  void resetDivision(int track);
  void resetPattern(int track);
  void resetProgrammed(int track);
  void resetEuclidean(int track);
  int calculateDivision(int divider, DividerType type);
  int euclidean(int length, int density);
  void build(int pattern[], int level, int counts[], int remainders[]);
  void build(int pattern[], int &step, int level, int counts[], int remainders[]);
  void rotate(int &pattern, int start, int end, int offset);
};

#endif
