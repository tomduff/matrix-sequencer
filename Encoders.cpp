#include "Encoders.h"

#define ONE_A 7
#define ONE_B 8
#define TWO_A 9
#define TWO_B 10
#define THREE_A 5
#define THREE_B 6

Encoders::Encoders()
  : one(ONE_A, ONE_B), two(TWO_A, TWO_B), three(THREE_A, THREE_B) {
}

void Encoders::initialise() {
  digitalWrite(ONE_A, HIGH);
  digitalWrite(ONE_B, HIGH);
  digitalWrite(TWO_A, HIGH);
  digitalWrite(TWO_B, HIGH);
  digitalWrite(THREE_A, HIGH);
  digitalWrite(THREE_B, HIGH);
}

EncoderEvent Encoders::event() {
  EncoderEvent event = EncoderEvent{Control::NoControl, EncoderState::Stopped};

  EncoderState state = read(one);
  if (state != EncoderState::Stopped) {
    event.control = Control::One;
    event.state = state;
  } else {
    state = read(two);
    if (state != EncoderState::Stopped) {
        event.control = Control::Two;
        event.state = state;
    } else {
      state = read(three);
      if (state != EncoderState::Stopped) {
          event.control = Control::Three;
          event.state = state;
      }
    }
  }

  return event;
}

EncoderState Encoders::read(Encoder &encoder) {
  EncoderState result = EncoderState::Stopped;
  int reading = encoder.read();
  if (reading < EncoderState::Decrement) {
    result = EncoderState::Decrement;
    encoder.write(EncoderState::Stopped);
  }
  else if (reading > EncoderState::Increment) {
    result = EncoderState::Increment;
    encoder.write(EncoderState::Stopped);
  }
  return result;
}
