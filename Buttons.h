#ifndef Buttons_h_
#define Buttons_h_

#include "Controller.h"

enum ButtonState {
  Released = 0,
  Clicked = 1,
  Held = 2
};

struct ButtonEvent {
  Control control;
  ButtonState state;
};

class Buttons : public Controller {
public:
  Buttons();
  virtual void initialise();
  ButtonEvent event();
  bool isHeld();
private:
  ButtonEvent readState();
  Control readButton();
  unsigned long holdStart;
  Control oldButton;
  ButtonState oldState;
};

#endif
