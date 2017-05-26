#ifndef Controller_h_
#define Controller_h_

#include "HardwareInterface.h"

enum Control {
  One = 0,
  Two = 1,
  Three = 2,
  NoControl = 99
};
enum ControlType {
  EncoderControl,
  ButtonControl
};
enum EncoderState {
  Stopped = 0,
  Increment = 1,
  Decrement = -1
};
enum ButtonState {
  Released = 0,
  Clicked = 1,
  Held = 2
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
