#ifndef Controller_h_
#define Controller_h_

#include "HardwareInterface.h"

enum Control {
  One = 0,
  Two = 1,
  Three = 2,
  NoControl = 99
};

class Controller : public HardwareInterface {
public:
private:
};

#endif
