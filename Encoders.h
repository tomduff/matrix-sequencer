#ifndef Encoders_h_
#define Encoders_h_

#include "Controller.h"
#include <Encoder.h>

enum EncoderState {
  Stopped = 0,
  Increment = 1,
  Decrement = -1
};

struct EncoderEvent {
  Control control;
  EncoderState state;
};

class Encoders : public Controller {
public:
  Encoders(bool reverse);
  virtual void initialise();
  EncoderEvent event();
private:
  Encoder one, two, three;
  EncoderState read(Encoder& encoder);
  bool reversed;
};

#endif
