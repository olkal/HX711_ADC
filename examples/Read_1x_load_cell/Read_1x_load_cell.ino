//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU  : Arduino Nano, ESP8266
//-------------------------------------------------------------------------------------
// This is an example sketch on how to use this library
// Settling time (number of samples) and data filtering can be adjusted in the config.h file

#include <HX711_ADC.h>
#include <EEPROM.h>

//HX711 constructor (dout pin, sck pin):
HX711_ADC LoadCell(4, 5);

const int eepromAdress = 0;

long t;

void setup() {
  
  float calValue; // calibration value
  calValue = 696.0; // uncomment this if you want to set this value in the sketch 
  #if defined(ESP8266) 
  //EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch the value from eeprom
  #endif
  //EEPROM.get(eepromAdress, calValue); // uncomment this if you want to fetch the value from eeprom
  
  Serial.begin(9600); delay(10);
  Serial.println();
  Serial.println("Starting...");
  LoadCell.begin();
  long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
  LoadCell.start(stabilisingtime);
  if(LoadCell.getTareTimeoutFlag()) {
    Serial.println("Tare timeout, check MCU>HX711 wiring and pin designations");
  }
  else {
    LoadCell.setCalFactor(calValue); // set calibration value (float)
    Serial.println("Startup + tare is complete");
  }
}

void loop() {
  //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  //use of delay in sketch will reduce effective sample rate (be carefull with use of delay() in the loop)
  LoadCell.update();

  //get smoothed value from data set
  if (millis() > t + 250) {
    float i = LoadCell.getData();
    Serial.print("Load_cell output val: ");
    Serial.println(i);
    t = millis();
  }

  //receive from serial terminal
  if (Serial.available() > 0) {
    float i;
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  //check if last tare operation is complete
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }

}
