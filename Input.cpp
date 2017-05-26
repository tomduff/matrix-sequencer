#include "Input.h"
#include <Arduino.h>

#define HYSTERIA 100
#define MAX_READING 1024



Input::Input(int io)
  : pin(io), previous(Signal::Low) {
}

void Input::initialise() {
		pinMode(pin, INPUT);
}

Signal Input::signal() {
	Signal current = Signal::Low;
	int reading = analogRead(pin);
	switch(previous) {
		case Low:
	  case Falling:
			if(reading > HYSTERIA) current = Signal::Rising;
			break;
		case High:
	  case Rising:
			if(reading > HYSTERIA) current = Signal::High;
		  else if(reading < HYSTERIA) current = Signal::Falling;
		  break;

	}
	previous = current;
	return current;
}
