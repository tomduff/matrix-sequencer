#ifndef Output_h_
#define Output_h_

#include "HardwareInterface.h"
#include "Io.h"

enum OutMode {
  Trigger,
  Gate,
  Clock
};

class Output : public HardwareInterface {
public:
  Output(int io);
  virtual void initialise();
  int signal(Signal signal, int step);
  void setMode(OutMode outMode);
private:
  int pin;
  OutMode mode;
  unsigned long triggerStart;
  int handleTrigger(Signal signal);
};

#endif
