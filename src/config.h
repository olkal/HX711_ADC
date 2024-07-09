/**
   -------------------------------------------------------------------------------------
   @file config.h
   
   HX711_ADC
   @brief Arduino library for HX711 24-Bit Analog-to-Digital Converter for Weight Scales
   @author Olav Kallhovd
   @date sept2017
   -------------------------------------------------------------------------------------
*/

/**
HX711_ADC configuration

Allowed values for "SAMPLES" is 1, 2, 4, 8, 16, 32, 64 or 128.
Higher value = improved filtering/smoothing of returned value, but longer setteling time and increased memory usage
Lower value = visa versa

The settling time can be calculated as follows:
Settling time = SAMPLES + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE / SPS

Example on calculating settling time using the values SAMPLES = 16, IGN_HIGH_SAMPLE = 1, IGN_LOW_SAMPLE = 1, and HX711 sample rate set to 10SPS:
(16+1+1)/10 = 1.8 seconds settling time.

Note that you can also overide (reducing) the number of samples in use at any time with the function: setSamplesInUse(samples).

*/

#ifndef HX711_ADC_config_h
#define HX711_ADC_config_h

//! number of samples in moving average dataset, value must be 1, 2, 4, 8, 16, 32, 64 or 128.
#define SAMPLES 					16		//!< default value: 16

//! adds extra sample(s) to the dataset and ignore peak high/low sample, value must be 0 or 1.
#define IGN_HIGH_SAMPLE 			1		//!< default value: 1
#define IGN_LOW_SAMPLE 				1		//!< default value: 1

//microsecond delay after writing sck pin high or low. This delay could be required for faster mcu's.
//Mcu's reported to need this delay is the ESP32 (issue #35) and RP2040, the Arduino Due and ESP8266 seems to run fine without it.
//Change the value to '0' to disable the delay.
#define SCK_DELAY					1		//default value: 1 (enabled)

//if you have some other time consuming (>60μs) interrupt routines that trigger while the sck pin is high, this could unintentionally set the HX711 into "power down" mode
//if required you can change the value to '1' to disable interrupts when writing to the sck pin.
#define SCK_DISABLE_INTERRUPTS		0		//default value: 0

#endif
