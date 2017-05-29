#include "Buttons.h"
#include <Arduino.h>

#define HOLD_TIME 500
#define SWITCH_TWO 100
#define SWITCH_ONE 200
#define SWITCH_THREE 400
#define BUTTON_PIN 2


Buttons::Buttons()
  : holdStart(0), oldButton(Control::NoControl) {
}

void Buttons::initialise() {
}

ButtonEvent Buttons::event() {
  ButtonEvent state = ButtonEvent{Control::NoControl, ButtonState::Released};
  Control currentButton = readButton();

  // A new button has been clicked so start the hold timer
  if (currentButton != Control::NoControl && (oldButton == Control::NoControl || oldButton != currentButton))  {
    holdStart = millis();
  }
  // If a button is now released then a state change has occurred
  else if (currentButton == Control::NoControl && oldButton != Control::NoControl) {
    state.control = oldButton;
    state.state = isHeld() ? ButtonState::Held : ButtonState::Clicked;
    holdStart = 0;
  }

  oldButton = currentButton;
  return state;
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
