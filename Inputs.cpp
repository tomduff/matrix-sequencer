#include "Inputs.h"
#include <Arduino.h>

#define ONE 0
//#define TWO 0

Inputs::Inputs() {
}

void Inputs::initialise() {
		pinMode(ONE, INPUT);
		//pinMode(TWO, INPUT);
}
