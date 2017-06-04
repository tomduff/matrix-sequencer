#include "Tracks.h"
#include "Utilities.h"
#include <Arduino.h>
#include <EEPROM.h>

#define CONFIG_VERSION 106
#define CONFIG_ADDRESS 0
#define MAX_STEP_INDEX 15
#define MAX_DIVIDER 7
#define TRACKS 3

Tracks::Tracks() {
  load();
  reset();
}

void Tracks::updatePattern(int track, int position) {
  int step = tracks[track].start + position;
  bitWrite(tracks[track].pattern, step, !bitRead(tracks[track].pattern, step));
  resetPattern(track);
  change = true;
}

void Tracks::rotatePattern(int track, int offset) {
  rotate(tracks[track].pattern, tracks[track].start, tracks[track].end, offset);
  resetPattern(track);
  change = true;
}

void Tracks::setStart(int track, int offset) {
  tracks[track].start += offset;
  Utilities::bound(tracks[track].start, 0, tracks[track].end);
  resetLength(track);
  resetPattern(track);
  change = true;
}

void Tracks::setEnd(int track, int offset) {
  tracks[track].end += offset;
  Utilities::bound(tracks[track].end, tracks[track].start, MAX_STEP_INDEX);
  resetLength(track);
  resetPattern(track);
  change = true;
}

void Tracks::setLength(int track, int offset) {
  tracks[track].length += offset;
  Utilities::bound(tracks[track].length, 0, MAX_STEP_INDEX);
  Utilities::bound(tracks[track].offset, 0, tracks[track].length);
  Utilities::bound(tracks[track].density, 0, tracks[track].length);
  resetLength(track);
  resetPattern(track);
  change = true;
}

void Tracks::setDensity(int track, int offset) {
  tracks[track].density += offset;
  Utilities::bound(tracks[track].density, 0, tracks[track].length);
  resetPattern(track);
  change = true;
}

void Tracks::setOffset(int track, int offset) {
  tracks[track].offset += offset;
  Utilities::cycle(tracks[track].offset, 0, tracks[track].length);
  resetPattern(track);
  change = true;
}

void Tracks::nextPatternType(int track) {
  int mode = (int)tracks[track].patternType;
  ++mode;
  Utilities::cycle(mode, PatternType::Programmed, PatternType::Euclidean);
  tracks[track].patternType = (PatternType) mode;
  resetLength(track);
  resetPattern(track);
  change = true;
}

void Tracks::setPlayMode(int track, int offset) {
  int mode = (int) tracks[track].play;
  mode += offset;
  Utilities::cycle(mode, PlayMode::Forward, PlayMode::Random);
  tracks[track].play = (PlayMode) mode;
  state[track].forward = true;
  change = true;
}

void Tracks::setOutMode(int track, int offset) {
  int mode = (int) tracks[track].out;
  mode += offset;
  Utilities::cycle(mode, OutMode::Trigger, OutMode::Gate);
  tracks[track].out = (OutMode) mode;
  change = true;
}

void Tracks::setDivider(int track, int offset) {
  tracks[track].divider += offset;
  Utilities::bound(tracks[track].divider, 0, MAX_DIVIDER);
  resetDivision(track);
  change = true;
}

void Tracks::nextDividerType(int track) {
  int mode = (int)tracks[track].dividerType;
  ++mode;
  Utilities::cycle(mode, DividerType::Beat, DividerType::Triplet);
  tracks[track].dividerType = (DividerType) mode;
  resetDivision(track);
  change = true;
}

int Tracks::getStart(int track) {
  return track < TRACKS ? tracks[track].start : getStart(0);
}

int Tracks::getEnd(int track) {
  return track < TRACKS ? tracks[track].end : getEnd(0);
}

int Tracks::getLength(int track) {
  return track < TRACKS ? state[track].length : getLength(0);
}

int Tracks::getPattern(int track) {
  return track < TRACKS ? state[track].pattern : getPattern(0);
}

int Tracks::getDivider(int track) {
  return track < TRACKS ? tracks[track].divider : getDivider(0);
}

DividerType Tracks::getDividerType(int track) {
  return track < TRACKS ? tracks[track].dividerType : getDividerType(0);
}

int Tracks::getPosition(int track) {
  return track < TRACKS ? state[track].position : getPosition(0);
}

int Tracks::getStep(int track) {
  return track < TRACKS ? bitRead(state[track].pattern, state[track].position) : !getStep(0);
}

PatternType Tracks::getPatternType(int track) {
  return track < TRACKS ? tracks[track].patternType : getPatternType(0);
}

PlayMode Tracks::getPlayMode(int track) {
  return track < TRACKS ? tracks[track].play : getPlayMode(0);
}

OutMode Tracks::getOutMode(int track) {
  return track < TRACKS ? tracks[track].out: getOutMode(0);
}

void Tracks::stepOn() {
  for(int track = 0; track < TRACKS; ++track) stepOn(track);
}

void Tracks::reset() {
  for(int track = 0; track < TRACKS; ++track) initialiseState(track);
}

void Tracks::stepOn(int track) {
  ++state[track].beat;
  if (state[track].beat >= state[track].division) {
    state[track].beat = 0;
    stepPosition(track);
  }
}

void Tracks::stepPosition(int track) {
  switch(tracks[track].play) {
    case Forward:
      ++state[track].position;
      Utilities::cycle(state[track].position, 0, state[track].length);
    break;
    case Backward:
      --state[track].position;
      Utilities::cycle(state[track].position, 0, state[track].length);
    break;
    case Random:
      state[track].position = random(0, state[track].length + 1);
    break;
    case Pendulum:
      if (state[track].forward) ++state[track].position;
      else --state[track].position;
      if (Utilities::reverse(state[track].position, 0, state[track].length)) state[track].forward  = !state[track].forward ;
    break;
  }
}

void Tracks::load() {
  Settings settings;
  EEPROM.get(CONFIG_ADDRESS, settings);
  if(settings.version != CONFIG_VERSION) for(int track = 0; track < TRACKS; ++ track) initialiseTrack(track);
  else for(int track = 0; track < TRACKS; ++ track) tracks[track] = settings.tracks[track];
}

void Tracks::save() {
  if (change) {
    Settings settings = {{tracks[0], tracks[1], tracks[2]}, CONFIG_VERSION};
    EEPROM.put(CONFIG_ADDRESS, settings);
    change = false;
  }
}

void Tracks::initialiseTrack(int track) {
  tracks[track].pattern = 0;
  tracks[track].start = 0;
  tracks[track].end = MAX_STEP_INDEX;
  tracks[track].length = MAX_STEP_INDEX;
  tracks[track].density = 0;
  tracks[track].offset = 0;
  tracks[track].play = PlayMode::Forward;
  tracks[track].out = OutMode::Trigger;
  tracks[track].divider = 0;
  tracks[track].patternType = PatternType::Programmed;
  tracks[track].dividerType = DividerType::Beat;
}

void Tracks::initialiseState(int track) {
  state[track].position = 0;
  state[track].beat = 0;
  state[track].forward = true;
  resetLength(track);
  resetDivision(track);
  resetPattern(track);
}

void Tracks::resetLength(int track) {
  switch(tracks[track].patternType) {
    case Programmed:
      state[track].length = tracks[track].end - tracks[track].start;
      break;
    case Euclidean:
      state[track].length = tracks[track].length;
      break;
  }
}

void Tracks::resetPattern(int track) {
  switch(tracks[track].patternType) {
    case Programmed:
      resetProgrammed(track);
      break;
    case Euclidean:
      resetEuclidean(track);
      break;
  }
}

void Tracks::resetProgrammed(int track) {
  state[track].pattern = 0;
  int index = 0;
  for (int step = tracks[track].start; step <= tracks[track].end; ++step) {
    bitWrite(state[track].pattern, index, bitRead(tracks[track].pattern, step));
    ++index;
  }
}

void Tracks::resetEuclidean(int track) {
  state[track].pattern = 0;
  state[track].pattern = euclidean(tracks[track].length + 1, tracks[track].density + 1);
  if (tracks[track].offset > 0) rotate(state[track].pattern, 0, tracks[track].length, tracks[track].offset);
}

void Tracks::resetDivision(int track) {
    state[track].division = calculateDivision(tracks[track].divider, tracks[track].dividerType);
    state[track].beat = 0;
}

int Tracks::calculateDivision(int divider, DividerType type) {
  int division = 1;
  if (type == DividerType::Beat) division = 1 << divider;
  else if (type == DividerType::Triplet) division = (divider + 1) * 3;
  return division;
}

int Tracks::euclidean(int length, int density) {
  int euclidean = 0;
  if(density >= length) density = length;
  int level = 0;
  int divisor = length - density;
  int remainders[MAX_STEP_INDEX + 1];
  int counts[MAX_STEP_INDEX + 1];
  remainders[0] = density;
  do {
    counts[level] = divisor / remainders[level];
    remainders[level + 1] = divisor % remainders[level];
    divisor = remainders[level];
    ++level;
  } while (remainders[level] > 1);
  counts[level] = divisor;
  int pattern[MAX_STEP_INDEX + 1];
  build(pattern, level, counts, remainders);
  for(int step = 0; step < length; ++step) bitWrite(euclidean, step, pattern[step]);
  if(euclidean != 0) while(!bitRead(euclidean, 0)) rotate(euclidean, 0, length, 1);
  return euclidean;
}

void Tracks::build(int pattern[], int level, int counts[], int remainders[]) {
  int step = 0;
  build(pattern, step, level, counts, remainders);
}

void Tracks::build(int pattern[], int &step, int level, int counts[], int remainders[]) {
  if (level == -1) {
    pattern[step] = 0;
    ++step;
  } else if (level == -2)  {
    pattern[step] = 1;
    ++step;
  } else {
    for (int index = 0; index < counts[level]; ++index) build(pattern, step, level - 1, counts, remainders);
    if (remainders[level] != 0) build(pattern, step, level - 2, counts, remainders);
  }
}

void Tracks::rotate(int &pattern, int start, int end, int offset) {
  int original = pattern;
  int length = end - start;
  for(int index = start; index <= end; ++index) {
    int set = index + offset;
    if (set < start) set = set + length + 1;
    else if (set > end) set = set - length - 1;
    bitWrite(pattern, set, bitRead(original, index));
  }
}
