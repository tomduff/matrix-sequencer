#include "Outputs.h"
#include <Arduino.h>

#define ONE 11
#define TWO 12
#define THREE 13
#define FOUR 17

Outputs::Outputs() {
}

void Outputs::initialise() {
		pinMode(ONE, OUTPUT);
		pinMode(TWO, OUTPUT);
		pinMode(THREE, OUTPUT);
		pinMode(FOUR, OUTPUT);
}
