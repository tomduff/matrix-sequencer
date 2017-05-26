#include "Tracks.h"
#include "Utilities.h"
#include <Arduino.h>

#define MAX_STEP_INDEX 15

Tracks::Tracks()
  : tracks{Track{MAX_STEP_INDEX, 0, 0, TrackMode::Forward}, Track{MAX_STEP_INDEX, 0, 0, TrackMode::Forward}, Track{MAX_STEP_INDEX, 0, 0, TrackMode::Forward}} {
}

void Tracks::updatePattern(int track, int position) {
  bitWrite(tracks[track].pattern, position, !bitRead(tracks[track].pattern, position));
}

void Tracks::applyOffset(int track, int offset) {
  int original = tracks[track].pattern;
  for(int index = 0; index <= tracks[track].length; ++index) {
    int set = index + offset;
    if(set < 0) set = tracks[track].length;
    else if(set > tracks[track].length) set = 0;
    bitWrite(tracks[track].pattern, set, bitRead(original, index));
  }
}

void Tracks::setLength(int track, int offset) {
    tracks[track].length += offset;
    Utilities::bound(tracks[track].length, 0, MAX_STEP_INDEX);
}

int Tracks::getLength(int track) {
    return tracks[track].length;
}

int Tracks::getPattern(int track) {
    int pattern = tracks[track].pattern;
    for(int i = tracks[track].length + 1; i <= MAX_STEP_INDEX; ++i) bitClear(pattern, i);
    return pattern;
}

int Tracks::getPosition(int track) {
    return tracks[track].position;
}

int Tracks::getStep(int track) {
  return bitRead(tracks[track].pattern, tracks[track].position);
}

void Tracks::stepOn() {
  stepOn(&tracks[0]);
  stepOn(&tracks[1]);
  stepOn(&tracks[2]);
}

void Tracks::stepOn(Track* track) {
  switch(track->mode) {
    case Forward:
      ++track->position;
      Utilities::cycle(track->position, 0, track->length);
    break;
    case Backward:
      --track->position;
      Utilities::cycle(track->position, 0, track->length);
    break;
    case Random:
      track->position  = random(0, MAX_STEP_INDEX + 1);
    break;
  }
}
