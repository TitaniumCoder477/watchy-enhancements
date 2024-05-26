#include "Watchy_7_SEG.h"
#include "settings.h"

Watchy7SEG watchy(settings);


void setup() {
  Serial.begin(115200);
  Serial.println();
  watchy.init();
}

void loop() {
  
}



