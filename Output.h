#ifndef Output_h_
#define Output_h_

#include "HardwareInterface.h"
#include "Io.h"
#include "Tracks.h"

class Output : public HardwareInterface {
public:
  Output(int io);
  virtual void initialise();
  int signal(Signal signal, OutMode mode, int step);
private:
  int pin;
  OutMode mode;
  unsigned long triggerStart;
  int handleTrigger(Signal signal);
};

#endif
