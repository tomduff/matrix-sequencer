#include "Buttons.h"
#include <Arduino.h>

#define HOLD_TIME 500
#define SWITCH_TWO 100
#define SWITCH_ONE 200
#define SWITCH_THREE 400
#define BUTTON_PIN 2


Buttons::Buttons()
  : holdStart(0), oldButton(Control::NoControl), oldState(ButtonState::Released) {
}

void Buttons::initialise() {
}

ButtonEvent Buttons::event() {
  ButtonEvent event = ButtonEvent{Control::NoControl, ButtonState::Released};
  Control button = readButton();
  ButtonState state = ButtonState::Released;
  if(button != Control::NoControl) {
    if (button != oldButton) {
      holdStart = millis();
    } else if (button == oldButton && isHeld()) {
      state = ButtonState::Held;
      event.control = button;
      if(oldState != ButtonState::Held) {
        event.state = state;
      }
    }
  } else if (oldState != ButtonState::Held) {
      event.control = oldButton;
      event.state = ButtonState::Clicked;
      holdStart = 0;
  }
  oldButton = button;
  oldState = state;
  return event;
}

Control Buttons::readButton() {
  Control button = Control::NoControl;
  int reading = analogRead(BUTTON_PIN);
  if (reading > SWITCH_THREE ) button = Control::Three;
  else if (reading > SWITCH_ONE) button = Control::One;
  else if (reading > SWITCH_TWO) button = Control::Two;
  return button;
}

bool Buttons::isHeld() {
  return (holdStart != 0 && millis() - holdStart > HOLD_TIME);
}
