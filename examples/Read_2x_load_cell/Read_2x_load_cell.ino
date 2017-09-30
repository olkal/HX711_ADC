//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : 2x HX711 asian module on channel A and 2x YZC-133 3kg load cell
// Tested with MCU  : Arduino Nano
//-------------------------------------------------------------------------------------
// This is an example sketch on how to use two HX711 modules with simultanious operation. 
// Settling time and data filtering can be adjusted in the HX711_ADC.h file

#include <HX711_ADC.h>

//HX711 constructor (dout pin, sck pin, gain/port)
HX711_ADC LdCell_1(A0, A1, 128);
HX711_ADC LdCell_2(A2, A3, 128);

void setup() {
  Serial.begin(9600);
  long t = millis();
  byte ldcells = 2; //number of load cells
  byte ldcell_1_rdy = 0;
  byte ldcell_2_rdy = 0;
  long stabilisingtime = 4000; // tare preciscion can be improved by adding a few seconds of stabilising time 
  while((ldcell_1_rdy + ldcell_2_rdy) < ldcells) { //run startup, stabilization and tare, both modules
    ldcell_1_rdy = LdCell_1.startMulti(stabilisingtime);
    ldcell_2_rdy = LdCell_2.startMulti(stabilisingtime);
  }
  LdCell_1.setCalFactor(696.0); // user set calibration factor (float) 
  LdCell_2.setCalFactor(792.0); // user set calibration factor (float) 
  Serial.print("Startup + tare time:");
  Serial.println(millis() - t);
}

void loop() {
  //.getData should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  float i = LdCell_1.getData(); 
  float k = LdCell_2.getData();
  Serial.print("Ld_cell_1:");
  Serial.print(i);
  Serial.print("    Ld_cell_2:");
  Serial.println(k);
 delay(5);
}

