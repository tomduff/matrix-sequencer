#include "Tracks.h"
#include "Utilities.h"
#include <Arduino.h>
#include <EEPROM.h>

#define CONFIG_VERSION 101
#define CONFIG_ADDRESS 0
#define MAX_STEP_INDEX 15

Tracks::Tracks() {
  load();
  reset();
}


void Tracks::updatePattern(int track, int position) {
  bitWrite(settings.tracks[track].pattern, position, !bitRead(settings.tracks[track].pattern, position));
  change = true;
}

void Tracks::applyOffset(int track, int offset) {
  int original = settings.tracks[track].pattern;
  for(int index = 0; index <= settings.tracks[track].length; ++index) {
    int set = index + offset;
    if(set < 0) set = settings.tracks[track].length;
    else if(set > settings.tracks[track].length) set = 0;
    bitWrite(settings.tracks[track].pattern, set, bitRead(original, index));
    change = true;
  }
}

void Tracks::setLength(int track, int offset) {
    settings.tracks[track].length += offset;
    Utilities::bound(settings.tracks[track].length, 0, MAX_STEP_INDEX);
    change = true;
}

void Tracks::setPlayMode(int track, PlayMode mode) {
  settings.tracks[track].play = mode;
  change = true;
}

void Tracks::nextPlayMode(int track) {
  int mode = (int)settings.tracks[track].play;
  ++mode;
  Utilities::cycle(mode, PlayMode::Forward, PlayMode::Random);
  settings.tracks[track].play = (PlayMode) mode;
  change = true;
}

void Tracks::setOutMode(int track, OutMode mode) {
  settings.tracks[track].out = mode;
  change = true;
}

void Tracks::nextOutMode(int track) {
  int mode = (int)settings.tracks[track].out;
  ++mode;
  Utilities::cycle(mode, OutMode::Trigger, OutMode::Gate);
  settings.tracks[track].out = (OutMode) mode;
  change = true;
}

int Tracks::getLength(int track) {
    return settings.tracks[track].length;
}

int Tracks::getPattern(int track) {
    int pattern = settings.tracks[track].pattern;
    for (int i = settings.tracks[track].length + 1; i <= MAX_STEP_INDEX; ++i) bitClear(pattern, i);
    return pattern;
}

int Tracks::getPosition(int track) {
    return settings.tracks[track].position;
}

int Tracks::getStep(int track) {
  return bitRead(settings.tracks[track].pattern, settings.tracks[track].position);
}

PlayMode Tracks::getPlayMode(int track) {
  return settings.tracks[track].play;
}

OutMode Tracks::getOutMode(int track) {
  return settings.tracks[track].out;
}

void Tracks::stepOn() {
  stepOn(&settings.tracks[0]);
  stepOn(&settings.tracks[1]);
  stepOn(&settings.tracks[2]);
}

void Tracks::reset() {
  settings.tracks[0].position = 0;
  settings.tracks[1].position = 0;
  settings.tracks[2].position = 0;
}

void Tracks::stepOn(Track* track) {
  switch(track->play) {
    case Forward:
      ++track->position;
      Utilities::cycle(track->position, 0, track->length);
    break;
    case Backward:
      --track->position;
      Utilities::cycle(track->position, 0, track->length);
    break;
    case Random:
      track->position  = random(0, track->length + 1);
    break;
  }
}

void Tracks::load() {
  EEPROM.get(CONFIG_ADDRESS, settings);
  if(settings.version != CONFIG_VERSION) initialise();
}

void Tracks::save() {
  if(change) {
    EEPROM.put(CONFIG_ADDRESS, settings);
    change = false;
  }
}

void Tracks::initialise() {
  settings.tracks[0] = {MAX_STEP_INDEX, 0, 0, PlayMode::Forward, OutMode::Trigger};
  settings.tracks[1] = {MAX_STEP_INDEX, 0, 0, PlayMode::Forward, OutMode::Trigger};
  settings.tracks[2] = {MAX_STEP_INDEX, 0, 0, PlayMode::Forward, OutMode::Trigger};
  settings.version = CONFIG_VERSION;

}
