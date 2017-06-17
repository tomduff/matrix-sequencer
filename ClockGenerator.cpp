#include "ClockGenerator.h"
#include "Utilities.h"
#include <Arduino.h>

#define bpmToMs(s) 60000L/s
#define pulseMs(ms,w) (ms/100L) * w
#define widthPercent(w) (w*3)+3

#define SPEED_STEP 1
#define MIN_SPEED 60L
#define MAX_SPEED 240L
#define WIDTH_STEP 1
#define MIN_WIDTH 0
#define MAX_WIDTH 30
#define MIN_MULTIPLIER 0
#define MAX_MULTIPLIER 4

ClockGenerator::ClockGenerator()
 : last(millis()), speed(120), width(0), running(false) {
}

Signal ClockGenerator::tick() {
  Signal signal = Signal::Low;
  unsigned long bpm = speed * (1L << multiplier);
  if (running) {
    unsigned long now = millis();
    unsigned long interval = bpmToMs(bpm);
    if (now - last > interval) {
      signal = Signal::Rising;
      last = now;
    } else if (now - last < pulseMs(interval, widthPercent(width))) {
      signal = Signal::High;
    }
  }
  return signal;
}

void ClockGenerator::setSpeed(int offset) {
  speed += offset * SPEED_STEP;
  Utilities::bound(speed, MIN_SPEED, MAX_SPEED);
}

void ClockGenerator::setWidth(int offset) {
  width += offset * WIDTH_STEP;
  Utilities::bound(width, MIN_WIDTH, MAX_WIDTH);
}

void ClockGenerator::setMulitplier(int offset) {
  multiplier += offset;
  Utilities::bound(multiplier, MIN_MULTIPLIER, MAX_MULTIPLIER);
}

int ClockGenerator::getSpeed() {
  return speed;
}

int ClockGenerator::getWidth() {
  return width;
}

int ClockGenerator::getMulitplier() {
  return multiplier;
}

void ClockGenerator::start() {
  reset();
  running = true;
}

void ClockGenerator::stop() {
  running = false;
}

void ClockGenerator::reset() {
  last = 0;
}

bool ClockGenerator::isRunning() {
  return running;
}
