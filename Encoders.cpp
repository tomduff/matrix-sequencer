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

ControlEvent Encoders::event() {
  ControlEvent event = ControlEvent{Control::NONE, ControlType::ENCODER, EncoderState::STOPPED};

  EncoderState state = read(one);
  if (state != EncoderState::STOPPED) {
    event = ControlEvent{Control::ONE, ControlType::ENCODER, state};
  } else {
    state = read(two);
    if (state != EncoderState::STOPPED) {
      event = ControlEvent{Control::TWO, ControlType::ENCODER, state};
    } else {
      state = read(three);
      if (state != EncoderState::STOPPED) {
        event = ControlEvent{Control::THREE, ControlType::ENCODER, state};
      }
    }
  }

  return event;
}

EncoderState Encoders::read(Encoder &encoder) {
  EncoderState result = EncoderState::STOPPED;
  int reading = encoder.read();
  if (reading < EncoderState::DECREMENT) {
    result = EncoderState::DECREMENT;
    encoder.write(EncoderState::STOPPED);
  }
  else if (reading > EncoderState::INCREMENT) {
    result = EncoderState::INCREMENT;
    encoder.write(EncoderState::STOPPED);
  }
  return result;
}
