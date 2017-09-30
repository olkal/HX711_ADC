//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU	: Arduino Nano
//-------------------------------------------------------------------------------------

#include <Arduino.h>
#include <HX711_ADC.h>

HX711_ADC::HX711_ADC(uint8_t dout, uint8_t sck, uint8_t gain) //constructor
{ 
	doutPin = dout;
	sckPin = sck;
	setGain(gain);
} 

void HX711_ADC::setGain(uint8_t gain)  //value should be 32, 64 or 128*
{
	if(gain < 64) GAIN = 2; //32, channel B
	else if(gain < 128) GAIN = 3; //64, channel A
	else GAIN = 1; //128, channel A
}

/*  start(t): will do conversions continuously for 't' milliseconds. 
*   Running this for 1-5s before tare() seems to improve the tare accuracy */
void HX711_ADC::start(unsigned int t)
{
	pinMode(sckPin, OUTPUT);
	pinMode(doutPin, INPUT);
	if(t < 400) t = 400; //min time for HX711 to be stable
	while(millis() < t) {
		getData();
	}
	tare();
}	

int HX711_ADC::startMulti(unsigned int t)
{
	pinMode(sckPin, OUTPUT);
	pinMode(doutPin, INPUT);
	if(t < 400) t = 400; //min time for HX711 to be stable
	if(isFirst) {
		timeStamp = millis();
		isFirst = 0;
	}	
	if(millis() < timeStamp + t) {
		getData();
		return 0;
	}
	else {
		return tareF();
	}
}

void HX711_ADC::tare() 
{
	uint8_t rdy = 0;
	while(!rdy) {
		rdy = tareF();
	}
}

int HX711_ADC::tareF() // zero the scale, call repetably, returns 1 when finnished
{
	uint8_t times = 0;
	if (tareTimes < DATA_SET) {
		uint8_t dout = digitalRead(doutPin); //check if conversion is ready
		if (!dout) {
			conversion24bit();
			tareTimes++;
		}
		return 0;
	}
	else {
		long i = smoothedData();
		tareOffset = i >> divBit;
		return 1;
	}
}

void HX711_ADC::setCalFactor(float cal) //raw data is divided by this value to convert to readable data
{
	calFactor = cal;
}

float HX711_ADC::getCalFactor() //raw data is divided by this value to convert to readable data
{
	return calFactor;
}

float HX711_ADC::getData() // return fresh data from the moving average data set
{
	long k = 0;
	long data = 0;
	byte dout = digitalRead(doutPin); //check if conversion is ready
	if (!dout) {
		conversion24bit();
	}
	data = smoothedData();
	data = (data >> divBit) - tareOffset;
	float x = (float)data / calFactor;
	return x;
}

long HX711_ADC::smoothedData() 
{
	long data = 0;
	long L = 0xFFFFFF;
	long H = 0x00;
	for (uint8_t r = 0; r < DATA_SET; r++) {
		if (L > dataSampleSet[r]) L = dataSampleSet[r]; // find lowest value
		if (H < dataSampleSet[r]) H = dataSampleSet[r]; // find highest value
		data += dataSampleSet[r];
	}
	if(IGN_LOW_SAMPLE) data -= L; //remove lowest value
	if(IGN_HIGH_SAMPLE) data -= H; //remove highest value
	return data;
}

long HX711_ADC::conversion24bit()  //returns 24 bit data and starts the next conversion
{
	unsigned long data = 0;
	uint8_t dout;
	for (uint8_t i = 0; i < (24 + GAIN); i++) { //read 24 bit data + set gain and start next conversion
		delayMicroseconds(1); // required for faster mcu's?
		digitalWrite(sckPin, 1);
		delayMicroseconds(1); 
		if (i < (24)) {
			dout = digitalRead(doutPin);
			data = data << 1;
			if (dout) {
			data++;
			}
		}
		digitalWrite(sckPin, 0);
	}
	data = data ^ 0x800000; // if out of range (min), change to 0
	// handle new converion value
	if (readIndex == DATA_SET - 1) {
		readIndex = 0;
	}
	else {
		readIndex++;
	}
	dataSampleSet[readIndex] = (long)data;
	return (long)data;
}

void HX711_ADC::powerDown() 
{
	digitalWrite(sckPin, LOW);
	digitalWrite(sckPin, HIGH);
}

void HX711_ADC::powerUp() 
{
	digitalWrite(sckPin, LOW);
}

long HX711_ADC::getTareOffset() 
{
	return tareOffset;
}


//for testing only:
//if ready: returns 24 bit data and starts the next conversion, else returns -1
float HX711_ADC::getSingleConversion()  
{	
	long data = 0;
	byte dout = digitalRead(doutPin); //check if conversion is ready
	if (!dout) {
		data = conversion24bit();
		data = data - tareOffset;
		float x = (float) data/calFactor;
		return x;
	}
	else return -1;
}

