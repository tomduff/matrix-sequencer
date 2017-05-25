#ifndef Encoders_h_
#define Encoders_h_

#include "Controller.h"
#include <Encoder.h>

class Encoders : public Controller {
public:
  Encoders();
  virtual void initialise();
  virtual ControlEvent event();
private:
  Encoder one, two, three;
  EncoderState read(Encoder& encoder);
};

#endif
