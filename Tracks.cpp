#include "Tracks.h"
#include "Utilities.h"

#define MAX_STEP_INDEX 15

Tracks::Tracks()
  : tracks{Track{MAX_STEP_INDEX, 0}, Track{MAX_STEP_INDEX, 0}, Track{MAX_STEP_INDEX, 0}} {
}

void Tracks::updatePattern(int track, int position) {
  bitWrite(tracks[track].pattern, position, !bitRead(tracks[track].pattern, position));
}

void Tracks::offset(int track, int offset) {
  int original = tracks[track].pattern;
  for(int index = 0; index <= tracks[track].length; ++index) {
    int set = index + offset;
    if(set < 0) set = tracks[track].length;
    else if(set > tracks[track].length) set = 0;
    bitWrite(tracks[track].pattern, set, bitRead(original, index));
  }
}

void Tracks::updateLength(int track, int offset) {
    tracks[track].length += offset;
    Utilities::bound(tracks[track].length, 0, MAX_STEP_INDEX);
}

int Tracks::length(int track) {
    return tracks[track].length;
}

int Tracks::pattern(int track) {
    int pattern = tracks[track].pattern;
    for(int i = tracks[track].length + 1; i <= MAX_STEP_INDEX; ++i) bitClear(pattern, i);
    return pattern;
}
