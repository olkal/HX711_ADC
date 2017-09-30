//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU  : Arduino Nano
//-------------------------------------------------------------------------------------
// This is an example sketch on how to use the library.
// Settling time and data filtering can be adjusted in the HX711_ADC.h file

#include <HX711_ADC.h>

//HX711 constructor (dout pin, sck pin, gain/port)
HX711_ADC LdCell(A0, A1, 128);

void setup() {
  Serial.begin(9600);
  long t = millis();
  long stabilisingtime = 4000; // tare precision can be improved by adding a few seconds of stabilising time 
  LdCell.start(stabilisingtime);
  LdCell.setCalFactor(696.0); // user set calibration factor (float)
  Serial.print("Startup + tare time:");
  Serial.println(millis() - t);
}

void loop() {
  //.getData should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  float i = LdCell.getData(); 
  Serial.print("Ld_cell_1: ");
  Serial.println(i);
  delay(5);
}

