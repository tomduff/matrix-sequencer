#include "Shuffle.h"
#include "Utilities.h"
#include <Arduino.h>

#define shuffleDelay(g,s) (g/96L) * s * 2L

#define BEATS 16

Shuffle::Shuffle()
 : gate(0), beat(0), state{0, Signal::Low, {Signal::Low, Signal::Low, Signal::Low, Signal::Low}, {false, false, false, false}} {
}

void Shuffle::clock(Signal signal) {
  unsigned long now = millis();
  if (signal == Signal::Rising) {
    gate = now - state.lastClock;
    state.lastClock = now;
    ++beat;
    Utilities::cycle(beat, 0, BEATS - 1);
    newCycle();
  }
  state.clockSignal = signal;
}

Signal Shuffle::tick(int track, int shuffle) {
  Signal signal;
  if(shuffle == 0 || gate == 0 || beat % 2 == 0) signal = state.clockSignal;
  else  signal = shuffleClock(track, shuffle);
  state.shuffleSignal[track] = signal;
  return signal;
}

void Shuffle::newCycle() {
  for(int track = 0; track < 4; ++track) state.newCycle[track] = true;
}

void Shuffle::reset() {
  beat = 0;
}

Signal Shuffle::shuffleClock(int track, int shuffle) {
  Signal signal = Signal::Low;
  if (!state.newCycle[track]) {
    signal = state.clockSignal;
  } else if ((millis() - state.lastClock) > shuffleDelay(gate, shuffle)) {
    if (state.shuffleSignal[track] == Signal::Low) {
      signal = Signal::Rising;
      state.newCycle[track] = false;
    }
  }
  return signal;
}
