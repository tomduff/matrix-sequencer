#ifndef Controller_h_
#define Controller_h_

#include "HardwareInterface.h"

enum Control {
  ONE = 0,
  TWO = 1,
  THREE = 2,
  NONE = 99
};

enum ControlType {
  ENCODER,
  BUTTON
};

enum EncoderState {
  STOPPED = 0,
  INCREMENT = 1,
  DECREMENT = -1
};

enum ButtonState {
  RELEASED = 0,
  CLICKED = 1,
  HELD = 2
};

struct ControlEvent {
  Control control;
  ControlType type;
  int state;
};

class Controller : public HardwareInterface {
public:
  virtual ControlEvent event() = 0;
private:
};

#endif
