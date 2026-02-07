#include <Arduino.h>
#include "SimpleLed.h"

SimpleLed::SimpleLed() {
    load();
}

void SimpleLed::load() {
    frequency = DEFAULT_FREQUENCY;
    pinMode(LEDPIN, OUTPUT);
}

void SimpleLed::Run() {
    
  if (frequency <= 25) frequency++;
  else frequency = 5;

  int flicker = floor((1000 / frequency) / 2);
  for (int i = 0; i < frequency; i++) {
      digitalWrite(LEDPIN, HIGH);
      delay(flicker);
      digitalWrite(LEDPIN, LOW);
      delay(flicker);
  }
  delay(200);
}

void SimpleLed::SetFrequency(int frequency) {
    frequency = frequency;
}