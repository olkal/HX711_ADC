//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU  : Arduino Nano
//-------------------------------------------------------------------------------------
/* This is an example sketch on how to find correct calibration factor for your HX711:
 * - Power up the scale and open Arduino serial terminal
 * - After stabelizing and tare is complete, put a known weight on the load cell
 * - Observe values on serial terminal
 * - Adjust the calibration factor until output value is same as your known weight:
 *    - Sending 'l' from the serial terminal decrease factor by 1.0
 *    - Sending 'L' from the serial terminal decrease factor by 10.0
 *    - Sending 'h' from the serial terminal increase factor by 1.0
 *    - Sending 'H' from the serial terminal increase factor by 10.0
 * - Observe and note the value of the new calibration factor
 * - Use this new calibration factor in your sketch 
*/ 

#include <HX711_ADC.h>

//HX711 constructor (dout pin, sck pin, gain/port)
HX711_ADC LdCell(A0, A1, 128);

void setup() {
  Serial.begin(9600);
  Serial.println("Wait...");
  long stabilisingtime = 4000; // tare preciscion can be improved by adding a few seconds of stabilising time
  LdCell.start(stabilisingtime);
  LdCell.setCalFactor(696.0); // user set calibration factor (float)
  Serial.println("Startup + tare is complete");
}

void loop() {
  //send 
  if (Serial.available() > 0) {
    float i;
    char inByte = Serial.read();
    if(inByte == 'l') i = -1.0;
    else if(inByte == 'L') i = -10.0;
    else if(inByte == 'h') i = 1.0;
    else if(inByte == 'H') i = 10.0;
    if(i != 0) {
      float v = LdCell.getCalFactor() + i;
      LdCell.setCalFactor(v);
    }
  }
  //.getData() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  float i = LdCell.getData();
  float v = LdCell.getCalFactor();
  Serial.print("Ld_cell output val: ");
  Serial.print(i);
  Serial.print("  Ld_cell calFactor: ");
  Serial.println(v);
  delay(5);
}

