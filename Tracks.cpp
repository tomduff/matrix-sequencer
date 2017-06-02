#include "Tracks.h"
#include "Utilities.h"
#include <Arduino.h>
#include <EEPROM.h>

#define CONFIG_VERSION 104
#define CONFIG_ADDRESS 0
#define MAX_STEP_INDEX 15
#define MAX_DIVIDER 7
#define TRACKS 3

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
    Utilities::cycle(tracks[track].divider, 0, MAX_DIVIDER);
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

int Tracks::getLength(int track) {
    return track < TRACKS ? tracks[track].length : getLength(0);
}

int Tracks::getPattern(int track) {
    int pattern = 0;
    if( track < TRACKS ) {
      pattern = tracks[track].pattern;
      for (int i = tracks[track].length + 1; i <= MAX_STEP_INDEX; ++i) bitClear(pattern, i);
    } else {
      pattern = ~getPattern(0);
    }
    return pattern;
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
  return track < TRACKS ? bitRead(tracks[track].pattern, state[track].position) : !getStep(0);
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
      if (state[track].forward) ++state[track].position;
      else --state[track].position;
      state[track].position = Utilities::reverse(state[track].position, 0, tracks[track].length);
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
  tracks[track].length = MAX_STEP_INDEX;
  tracks[track].pattern = 0;
  tracks[track].type = TrackType::Euclidean;
  tracks[track].play = PlayMode::Forward;
  tracks[track].out = OutMode::Trigger;
  tracks[track].divider = 0;
  tracks[track].dividerType = DividerType::Beat;
}

void Tracks::initialiseState(int track) {
  state[track].position = 0;
  state[track].beat = 0;
  state[track].forward = true;
  resetDivision(track);
}

// unsigned int Tracks::euclidean(int n, int k, int o) { 
// 	int pauses = n - k;
// 	int pulses = k;
// 	int offset = o;
// 	int steps = n;
// 	int per_pulse = pauses / k;
// 	int remainder = pauses%pulses;
// 	unsigned int workbeat[n];
// 	unsigned int outbeat;
// 	int outbeat2;
// 	unsigned int working;
// 	int workbeat_count = n;
// 	int a;
// 	int b;
// 	int trim_count;
//
// 	for (a = 0; a < n; a++) {
// 		if (a < pulses) workbeat[a] = 1;
// 		else workbeat[a] = 0;
// 	}
//
// 	if (per_pulse > 0 && remainder < 2) {
// 		for (a = 0; a < pulses; a++) {
// 			for (b = workbeat_count - 1; b > workbeat_count - per_pulse - 1; b--) workbeat[a] = ConcatBin(workbeat[a], workbeat[b]);
// 			workbeat_count = workbeat_count - per_pulse;
// 		}
// 		outbeat = 0;
// 		for (a = 0; a < workbeat_count; a++) outbeat = ConcatBin(outbeat, workbeat[a]);
// 		if (offset > 0) outbeat2 = rightRotate(offset, outbeat, steps);
// 		else outbeat2 = outbeat;
// 		return outbeat2;
// 	} else {
// 		if (pulses == 0) { pulses = 1; }
// 		int groupa = pulses;
// 		int groupb = pauses;
// 		int iteration = 0;
//
// 		while (groupb > 1) {
// 			if (groupa > groupb) {
// 				int a_remainder = groupa - groupb;
// 				trim_count = 0;
// 				for (a = 0; a < groupa - a_remainder; a++) {
// 					workbeat[a] = ConcatBin(workbeat[a], workbeat[workbeat_count - 1 - a]);
// 					trim_count++;
// 				}
// 				workbeat_count = workbeat_count - trim_count;
// 				groupa = groupb;
// 				groupb = a_remainder;
// 			} else if (groupb > groupa) {
// 				int b_remainder = groupb - groupa;
// 				trim_count = 0;
// 				for (a = workbeat_count - 1; a >= groupa + b_remainder; a--) {
// 					workbeat[workbeat_count - a - 1] = ConcatBin(workbeat[workbeat_count - a - 1], workbeat[a]);
// 					trim_count++;
// 				}
// 				workbeat_count = workbeat_count - trim_count;
// 				groupb = b_remainder;
// 			} else if (groupa == groupb) {
// 				trim_count = 0;
// 				for (a = 0; a < groupa; a++) {
// 					workbeat[a] = ConcatBin(workbeat[a], workbeat[workbeat_count - 1 - a]);
// 					trim_count++;
// 				}
// 				workbeat_count = workbeat_count - trim_count;
// 				groupb = 0;
// 			}
// 			iteration++;
// 		}
//
// 		outbeat = 0;
// 		for (a = 0; a < workbeat_count; a++) outbeat = ConcatBin(outbeat, workbeat[a]);
//
// 		if (offset > 0) outbeat2 = rightRotate(offset, outbeat, steps);
//     else outbeat2 = outbeat;
//
// 		return outbeat2;
// 	}
// }
//
// int Tracks::rightRotate(int shift, uint16_t value, uint8_t pattern_length) {
// 	uint16_t mask = ((1 << pattern_length) - 1);
// 	value &= mask;
// 	return ((value >> shift) | (value << (pattern_length - shift))) & mask;
// }
//
// int Tracks::findlength(unsigned int bnry) {
// 	boolean lengthfound = false;
// 	int length = 1;
// 	for (int q = 32; q >= 0; q--) {
// 		int r = bitRead(bnry, q);
// 		if (r == 1 && lengthfound == false) {
// 			length = q + 1;
// 			lengthfound = true;
// 		}
// 	}
// 	return length;
// }
//
// unsigned int Tracks::ConcatBin(unsigned int bina, unsigned int binb) {
// 	int binb_len = findlength(binb);
// 	unsigned int sum = (bina << binb_len);
// 	return sum | binb;
// }
