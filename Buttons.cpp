#include "Buttons.h"
#include <Arduino.h>

#define HOLD_TIME 500
#define SWITCH_TWO 100
#define SWITCH_ONE 200
#define SWITCH_THREE 400
#define BUTTON_PIN 2


Buttons::Buttons()
  : holdStart(0), oldButton(Control::NONE) {
}

void Buttons::initialise() {
}

ControlEvent Buttons::event() {
  return readState();
}

ControlEvent Buttons::readState() {

  ControlEvent state = ControlEvent{Control::NONE, ControlType::BUTTON, ButtonState::RELEASED};
  Control currentButton = readButton();

  // A new button has been clicked so start the hold timer
  if (currentButton != Control::NONE && (oldButton == Control::NONE || oldButton != currentButton))  {
    holdStart = millis();
  }
  // If a button is now released then a state change has occurred
  else if (currentButton == Control::NONE && oldButton != Control::NONE) {
    state.control = oldButton;
    state.state = isHeld() ? ButtonState::HELD : ButtonState::CLICKED;
    holdStart = 0;
  }

  oldButton = currentButton;
  return state;
}

Control Buttons::readButton() {
  Control button = Control::NONE;
  int reading = analogRead(BUTTON_PIN);
  if (reading > SWITCH_THREE ) button = Control::THREE;
  else if (reading > SWITCH_ONE) button = Control::ONE;
  else if (reading > SWITCH_TWO) button = Control::TWO;
  return button;
}

bool Buttons::isHeld() {
  return (holdStart != 0 && millis() - holdStart > HOLD_TIME);
}
