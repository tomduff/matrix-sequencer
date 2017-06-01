#include "Tracks.h"
#include "Utilities.h"
#include <Arduino.h>
#include <EEPROM.h>

#define CONFIG_VERSION 103
#define CONFIG_ADDRESS 0
#define MAX_STEP_INDEX 15

Tracks::Tracks() {
  load();
  reset();
}


void Tracks::updatePattern(int track, int position) {
  bitWrite(tracks[track].pattern, position, !bitRead(tracks[track].pattern, position));
  change = true;
}

void Tracks::applyOffset(int track, int offset) {
  int original = tracks[track].pattern;
  for(int index = 0; index <= tracks[track].length; ++index) {
    int set = index + offset;
    if(set < 0) set = tracks[track].length;
    else if(set > tracks[track].length) set = 0;
    bitWrite(tracks[track].pattern, set, bitRead(original, index));
    change = true;
  }
}

void Tracks::setLength(int track, int offset) {
    tracks[track].length += offset;
    Utilities::bound(tracks[track].length, 0, MAX_STEP_INDEX);
    change = true;
}

void Tracks::setPlayMode(int track, PlayMode mode) {
  tracks[track].play = mode;
  state[track].direction = PlayMode::Forward;
  change = true;
}

void Tracks::nextPlayMode(int track) {
  int mode = (int)tracks[track].play;
  ++mode;
  Utilities::cycle(mode, PlayMode::Forward, PlayMode::Pendulum);
  tracks[track].play = (PlayMode) mode;
  state[track].direction = PlayMode::Forward;
  change = true;
}

void Tracks::setOutMode(int track, OutMode mode) {
  tracks[track].out = mode;
  change = true;
}

void Tracks::nextOutMode(int track) {
  int mode = (int)tracks[track].out;
  ++mode;
  Utilities::cycle(mode, OutMode::Trigger, OutMode::Gate);
  tracks[track].out = (OutMode) mode;
  change = true;
}

int Tracks::getLength(int track) {
    return track < 3 ? tracks[track].length : getLength(0);
}

int Tracks::getPattern(int track) {
    int pattern = 0;
    if( track < 3 ) {
      pattern = tracks[track].pattern;
      for (int i = tracks[track].length + 1; i <= MAX_STEP_INDEX; ++i) bitClear(pattern, i);
    } else {
      pattern = ~getPattern(0);
    }
    return pattern;
}

int Tracks::getPosition(int track) {
  return track < 3 ? state[track].position : getPosition(0);
}

int Tracks::getStep(int track) {
  return track < 3 ? bitRead(tracks[track].pattern, state[track].position) : !getStep(0);
}

PlayMode Tracks::getPlayMode(int track) {
  return track < 3 ? tracks[track].play : getPlayMode(0);
}

OutMode Tracks::getOutMode(int track) {
  return track < 3 ? tracks[track].out: getOutMode(0);
}

void Tracks::stepOn() {
  stepOn(0);
  stepOn(1);
  stepOn(2);
}

void Tracks::reset() {
  initialise(state[0]);
  initialise(state[1]);
  initialise(state[2]);
}

void Tracks::stepOn(int track) {
  switch(tracks[track].play) {
    case Forward:
      ++state[track].position;
      Utilities::cycle(state[track].position, 0, tracks[track].length);
    break;
    case Backward:
      --state[track].position;
      Utilities::cycle(state[track].position, 0, tracks[track].length);
    break;
    case Random:
      state[track].position = random(0, tracks[track].length + 1);
    break;
    case Pendulum:
      if (state[track].direction == PlayMode::Forward) {
        ++state[track].position;
        if (state[track].position > tracks[track].length) {
          state[track].direction = PlayMode::Backward;
          state[track].position = tracks[track].length;
        }
      } else {
        --state[track].position;
        if (state[track].position < 0) {
          state[track].direction = PlayMode::Forward;
          state[track].position = 0;
        }
      }
    break;
  }
}

void Tracks::load() {
  Settings settings;
  EEPROM.get(CONFIG_ADDRESS, settings);
  if(settings.version != CONFIG_VERSION) {
    initialise(tracks[0]);
    initialise(tracks[1]);
    initialise(tracks[2]);
  }
  else {
    tracks[0] = settings.tracks[0];
    tracks[1] = settings.tracks[1];
    tracks[2] = settings.tracks[2];
  }
  reset();
}

void Tracks::save() {
  if (change) {
    Settings settings = {{tracks[0], tracks[1], tracks[2]}, CONFIG_VERSION};
    EEPROM.put(CONFIG_ADDRESS, settings);
    change = false;
  }
}

void Tracks::initialise(Track& track) {
  track.length = MAX_STEP_INDEX;
  track.pattern = 0;
  track.type = TrackType::Euclidean;
  track.play = PlayMode::Forward;
  track.out = OutMode::Trigger;
  track.divider = ClockDivider::OneBeat;
}

void Tracks::initialise(TrackState& state) {
  state.position = 0;
  state.beat = 0;
  state.direction = PlayMode::Forward;
}
