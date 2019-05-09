//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU  : Arduino Nano
//-------------------------------------------------------------------------------------
// This is an example sketch on how to use this library for two ore more HX711 modules
// Settling time (number of samples) and data filtering can be adjusted in the config.h file

#include <HX711_ADC.h>
#include <EEPROM.h>

//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell_1(4, 5); //HX711 1
HX711_ADC LoadCell_2(6, 7); //HX711 2

const int eepromAdress_1 = 0; // eeprom adress for calibration value load cell 1 (4 bytes)
const int eepromAdress_2 = 4; // eeprom adress for calibration value load cell 2 (4 bytes)

long t;

void setup() {
  
  float calValue_1; // calibration value load cell 1
  float calValue_2; // calibration value load cell 2
  
  calValue_1 = 696.0; // uncomment this if you want to set this value in the sketch 
  calValue_2 = 733.0; // uncomment this if you want to set this value in the sketch 
  #if defined(ESP8266) 
  //EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch the value from eeprom
  #endif
  //EEPROM.get(eepromAdress_1, calValue_1); // uncomment this if you want to fetch the value from eeprom
  //EEPROM.get(eepromAdress_2, calValue_2); // uncomment this if you want to fetch the value from eeprom
  
  Serial.begin(9600); delay(10);
  Serial.println();
  Serial.println("Starting...");
  LoadCell_1.begin();
  LoadCell_2.begin();
  long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
  byte loadcell_1_rdy = 0;
  byte loadcell_2_rdy = 0;
  while ((loadcell_1_rdy + loadcell_2_rdy) < 2) { //run startup, stabilization and tare, both modules simultaniously
    if (!loadcell_1_rdy) loadcell_1_rdy = LoadCell_1.startMultiple(stabilisingtime);
    if (!loadcell_2_rdy) loadcell_2_rdy = LoadCell_2.startMultiple(stabilisingtime);
  }
  if (LoadCell_1.getTareTimeoutFlag()) {
    Serial.println("Tare timeout, check MCU>HX711 no.1 wiring and pin designations");
  }
  if (LoadCell_2.getTareTimeoutFlag()) {
    Serial.println("Tare timeout, check MCU>HX711 no.2 wiring and pin designations");
  }
  LoadCell_1.setCalFactor(calValue_1); // user set calibration value (float)
  LoadCell_2.setCalFactor(calValue_2); // user set calibration value (float)
  Serial.println("Startup + tare is complete");
}

void loop() {
  //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  //longer delay in scetch will reduce effective sample rate (be carefull with use of delay() in the loop)
  LoadCell_1.update();
  LoadCell_2.update();

  //get smoothed value from data set + current calibration factor
  if (millis() > t + 250) {
    float a = LoadCell_1.getData();
    float b = LoadCell_2.getData();
    Serial.print("Load_cell 1 output val: ");
    Serial.print(a);
    Serial.print("    Load_cell 2 output val: ");
    Serial.println(b);
    t = millis();
  }

  //receive from serial terminal
  if (Serial.available() > 0) {
    float i;
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCell_1.tareNoDelay();
      LoadCell_2.tareNoDelay();
    }
  }

  //check if last tare operation is complete
  if (LoadCell_1.getTareStatus() == true) {
    Serial.println("Tare load cell 1 complete");
  }
  if (LoadCell_2.getTareStatus() == true) {
    Serial.println("Tare load cell 2 complete");
  }

}
