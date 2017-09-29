//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU	: Arduino Nano
//-------------------------------------------------------------------------------------

#ifndef HX711_ADC_h
#define HX711_ADC_h

#include <Arduino.h>

#define SAMPLES 			16								// no of samples in moving average data set, value must be 4, 8, 16, 32 or 64
#define IGN_HIGH_SAMPLE 	1 								// adds one sample to the set and ignore peak high sample, value must be 0 or 1
#define IGN_LOW_SAMPLE 		1 								// adds one sample to the set and ignore peak low sample, value must be 0 or 1
#define TARE_X2 			1 								// if true, tare is performed twice and value is based on average of the 2 sets
#define TARE_SAMPLES 		8 								// no of sample readings in tare function, value should be => SAMPLES
#define DATA_SET 			SAMPLES + IGN_HIGH_SAMPLE + IGN_HIGH_SAMPLE // total samples in memory

#if (SAMPLES  != 4) & (SAMPLES  != 8) & (SAMPLES  != 16) & (SAMPLES  != 32) & (SAMPLES  != 64) & (SAMPLES  != 128)
	#error "number of SAMPLES not valid!"
#endif

#if (SAMPLES == 4)
#define DIVB 2
#elif  (SAMPLES == 8)
#define DIVB 3
#elif  (SAMPLES == 16)
#define DIVB 4
#elif  (SAMPLES == 32)
#define DIVB 5
#elif  (SAMPLES == 64)
#define DIVB 6
#elif  (SAMPLES == 128)
#define DIVB 7
#endif

class HX711_ADC
{
	public:
		HX711_ADC(uint8_t dout, uint8_t sck, uint8_t gain); //constructor
		void setGain(uint8_t gain = 128); //value should be 32, 64 or 128*
		void start(unsigned int t);
		void tare(); // zero the scale
		int tareF(); // zero the scale one conversion at a time
		int startMulti(unsigned int t);
		void setCalFactor(float cal); //raw data is divided by this value to convert to readable data
		float getData(); // returns data from the moving average data set 
		float getSingleConversion(); //for testing
		void powerDown(); 
		void powerUp(); 
		long getTareOffset();
	
	protected:
		long conversion24bit(); //if conversion is ready: returns 24 bit data and starts the next conversion
		long smoothedData();
		uint8_t sckPin; //HX711 pd_sck pin
		uint8_t doutPin; //HX711 dout pin
		uint8_t GAIN;
		float calFactor;
		long dataSampleSet[DATA_SET + 1]; // data set
		long tareOffset;
		long tareOffsetTemp;
		int readIndex;
		long timeStamp;
		uint8_t isFirst = 1;
		uint8_t tareTimes;
		const uint8_t divBit = DIVB;
};	

#endif
   