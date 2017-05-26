#ifndef Input_h_
#define Input_h_

#include "Io.h"
#include "HardwareInterface.h"

class Input : public HardwareInterface {
public:
  Input(int io);
  virtual void initialise();
  Signal signal();
private:
  int pin;
  Signal previous;
};

#endif
