#ifndef Buttons_h_
#define Buttons_h_

#include "Controller.h"

class Buttons : public Controller {
public:
  Buttons();
  virtual void initialise();
  virtual ControlEvent event();
  bool isHeld();
private:
  ControlEvent readState();
  Control readButton();
  unsigned long holdStart;
  Control oldButton;
};

#endif
