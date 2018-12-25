/*
HX711_ADC configuration

Allowed values for "SAMPLES" is 4, 8, 16, 32 or 64.
Higher value = improved filtering/smoothing of returned value, but longer setteling time and increased memory usage
Lower value = visa versa

The settling time can be calculated as follows:
Settling time = SAMPLES + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE / SPS

Example on calculating settling time using the values SAMPLES = 16, IGN_HIGH_SAMPLE = 1, IGN_LOW_SAMPLE = 1, and HX711 sample rate set to 10SPS:
(16+1+1)/10 = 1.8 seconds settling time

*/

#define SAMPLES 			16									// no of samples in moving average data set, value must be 4, 8, 16, 32 or 64
#define IGN_HIGH_SAMPLE 	1 									// adds one sample to the set and ignore peak high sample, value must be 0 or 1
#define IGN_LOW_SAMPLE 		1 									// adds one sample to the set and ignore peak low sample, value must be 0 or 1