//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU	: Arduino Nano
//-------------------------------------------------------------------------------------

#include <Arduino.h>
#include <HX711_ADC.h>


HX711_ADC::HX711_ADC(uint8_t dout, uint8_t sck) //constructor
{ 	
	doutPin = dout;
	sckPin = sck;
} 

void HX711_ADC::setGain(uint8_t gain)  //value should be 32, 64 or 128*
{
	if(gain < 64) GAIN = 2; //32, channel B
	else if(gain < 128) GAIN = 3; //64, channel A
	else GAIN = 1; //128, channel A
}

void HX711_ADC::begin()
{
	pinMode(sckPin, OUTPUT);
	pinMode(doutPin, INPUT);
	setGain(128);
	powerUp();
}


void HX711_ADC::begin(uint8_t gain)
{
	pinMode(sckPin, OUTPUT);
	pinMode(doutPin, INPUT);
	setGain(gain);
	powerUp();
}

/*  start(t): will do conversions continuously for 't' +400 milliseconds (400ms is min. settling time at 10SPS). 
*   Running this for 1-5s before tare() seems to improve the tare accuracy */
int HX711_ADC::start(unsigned int t)
{
	t += 400;
	while(millis() < t) 
	{
		getData();
		yield();
	}
	tare();
	tareStatus = 0;
}	

/*  startMultiple(t): use this if you have more than one load cell and you want to do tare and stabilization simultaneously.
	Will do conversions continuously for 't' +400 milliseconds (400ms is min. settling time at 10SPS). 
*   Running this for 1-5s before tare() seems to improve the tare accuracy */
int HX711_ADC::startMultiple(unsigned int t)
{
	tareTimeoutFlag = 0;
	static unsigned long timeout = millis() + tareTimeOut;
	if(startStatus == 0) {
		if(isFirst) {
			startMultipleTimeStamp = millis();
			if (t < 400) 
			{
				startMultipleWaitTime = t + 400; //min time for HX711 to be stable
			} 
			else 
			{
				startMultipleWaitTime = t;
			}
			isFirst = 0;
		}	
		if(millis() < startMultipleTimeStamp + startMultipleWaitTime) {
			update(); //do conversions during stabilization time
			yield();
			return 0;
		}
		else { //do tare after stabilization time is up
			doTare = 1;
			update();
			if(convRslt == 2) 
			{	
				doTare = 0;
				convRslt = 0;
				startStatus = 1;
			}
			if (!tareTimeoutDisable) 
			{
				if (millis() > timeout) 
				{ 
				tareTimeoutFlag = 1;
				return 1; // Prevent endless loop if no HX711 is connected
				}
			}
		}
	}
	return startStatus;
}

void HX711_ADC::tare() 
{
	uint8_t rdy = 0;
	doTare = 1;
	tareTimes = 0;
	tareTimeoutFlag = 0;
	unsigned long timeout = millis() + tareTimeOut;
	while(rdy != 2) 
	{
		rdy = update();
		if (!tareTimeoutDisable) 
		{
			if (millis() > timeout) 
			{ 
				tareTimeoutFlag = 1;
				break; // Prevent endless loop if no HX711 is connected
			}
		}
		yield();
	}
}

bool HX711_ADC::getTareStatus() 
{
	bool t = tareStatus;
	tareStatus = 0;
	return t;
}

void HX711_ADC::tareNoDelay() 
{
	doTare = 1;
	tareTimes = 0;
}

void HX711_ADC::setCalFactor(float cal) //raw data is divided by this value to convert to readable data
{
	calFactor = cal;
}

float HX711_ADC::getCalFactor() //raw data is divided by this value to convert to readable data
{
	return calFactor;
}

//call function update() in loop
//if conversion is ready; read out 24 bit data and add to data set, returns 1
//if tare operation is complete, returns 2
//else returns 0
uint8_t HX711_ADC::update() 
{
	byte dout = digitalRead(doutPin); //check if conversion is ready
	if (!dout) 
	{
		conversion24bit();
		//if(s) Serial.print(s);
	}
	else convRslt = 0;
	return convRslt;
}

float HX711_ADC::getData() // return fresh data from the moving average data set
{
	//long k = 0;
	long data = 0;
	//data = smoothedData() - tareOffset;
	lastSmoothedData = smoothedData();
	data = (lastSmoothedData >> divBit) - tareOffset ;
	//data = (data >> divBit);
	
	float x = (float)data / calFactor;
	return x;
}

long HX711_ADC::smoothedData() 
{
	long data = 0;
	long L = 0xFFFFFF;
	long H = 0x00;
	//for (uint8_t r = 0; r < DATA_SET; r++) {
	for (uint8_t r = 0; r < (samplesInUse + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE); r++) 
	{
		if (L > dataSampleSet[r]) L = dataSampleSet[r]; // find lowest value
		if (H < dataSampleSet[r]) H = dataSampleSet[r]; // find highest value
		data += dataSampleSet[r];
	}
	if(IGN_LOW_SAMPLE) data -= L; //remove lowest value
	if(IGN_HIGH_SAMPLE) data -= H; //remove highest value
	return data;
}

uint8_t HX711_ADC::conversion24bit()  //read 24 bit data and start the next conversion
{
	conversionTime = micros() - conversionStartTime;
	conversionStartTime = micros();
	unsigned long data = 0;
	uint8_t dout;
	convRslt = 0;
	if(SCK_DISABLE_INTERRUPTS) noInterrupts();
	for (uint8_t i = 0; i < (24 + GAIN); i++) 
	{ //read 24 bit data + set gain and start next conversion
		if(SCK_DELAY) delayMicroseconds(1); // could be required for faster mcu's, set value in config.h
		digitalWrite(sckPin, 1);
		if(SCK_DELAY) delayMicroseconds(1); // could be required for faster mcu's, set value in config.h
		if (i < (24)) 
		{
			dout = digitalRead(doutPin);
			data = data << 1;
			if (dout) 
			{
				data++;
			}
		}
		digitalWrite(sckPin, 0);
	}
	if(SCK_DISABLE_INTERRUPTS) interrupts(); 
	data = data ^ 0x800000; // if out of range (min), change to 0
	//if (readIndex == DATA_SET - 1) {
	if (readIndex == samplesInUse + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE - 1) 
	{
		readIndex = 0;
	}
	else 
	{
		readIndex++;
	}
	if(data > 0)  
	{
		convRslt++;
		dataSampleSet[readIndex] = (long)data;
		if(doTare) 
		{
			if (tareTimes < DATA_SET) 
			{
				tareTimes++;
			}
			else 
			{
				//tareOffset = smoothedData();
				tareOffset = (smoothedData() >> divBit);
				tareTimes = 0;
				doTare = 0;
				tareStatus = 1;
				convRslt++;
			}
		}
	}
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

//get the tare offset (raw data value output without the scale "CalFactor")
long HX711_ADC::getTareOffset() 
{
	return tareOffset;
}

//set new tare offset (raw data value input without the scale "CalFactor")
void HX711_ADC::setTareOffset(long newoffset)
{
	tareOffset = newoffset;
}

//for testing and debugging:
//returns current value of readIndex
int HX711_ADC::getReadIndex()
{
	return readIndex;
}

//for testing and debugging:
//returns latest conversion time in millis
float HX711_ADC::getConversionTime()
{
	return conversionTime/1000.0;
}

//for testing and debugging:
//returns the HX711 samples ea seconds based on the latest conversion time. 
//The HX711 can be set to 10SPS or 80SPS. For general use the recommended setting is 10SPS.
float HX711_ADC::getSPS()
{
	float sps = 1000000.0/conversionTime;
	return sps;
}

//for testing and debugging:
//returns the tare timeout flag from the last tare operation. 
//0 = no timeout, 1 = timeout
bool HX711_ADC::getTareTimeoutFlag() 
{
	return tareTimeoutFlag;
}

void HX711_ADC::disableTareTimeout()
{
	tareTimeoutDisable = 1;
}

long HX711_ADC::getSettlingTime() 
{
	long st = getConversionTime() * DATA_SET;
	return st;
}

//overide the number of samples in use
//value is rounded down to the nearest valid value
void HX711_ADC::setSamplesInUse(int samples)
{
	int old_value = samplesInUse;
	int old_divbit = divBit;
	
	if(samples <= SAMPLES)
	{
		if(samples == 0) {samplesInUse = SAMPLES; divBit = divBitCompiled;} //reset to the original value
		else if(samples == 1) {samplesInUse = 1; divBit = 0;}
		else if((samples > 1) && (samples < 4)) {samplesInUse = 2; divBit = 1;}
		else if((samples >= 4) && (samples < 8)) {samplesInUse = 4; divBit = 2;}
		else if((samples >= 8) && (samples < 16)) {samplesInUse = 8; divBit = 3;}
		else if((samples >= 16) && (samples < 32)) {samplesInUse = 16; divBit = 4;}
		else if((samples >= 32) && (samples < 64)) {samplesInUse = 32; divBit = 5;}
		else if((samples >= 64) && (samples < 128)) {samplesInUse = 64; divBit = 6;}
		else {samplesInUse = 128; divBit = 7;}
		
		//replace the value of all samples in use with the last conversion value
		if(samplesInUse != old_value) 
		{
			for (uint8_t r = 0; r < samplesInUse + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE; r++) 
			{
				dataSampleSet[r] = (lastSmoothedData >> old_divbit);
			}
			readIndex = 0;
		}
	}
}

//returns the current number of samples in use.
int HX711_ADC::getSamplesInUse()
{
	return samplesInUse;
}

