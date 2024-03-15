Latest release and change log here: https://github.com/olkal/HX711_ADC/releases

This an Arduino library for the HX711 24-bit ADC for weight scales.
Data retrieval from the HX711 is done without blocking the mcu, also on the 10SPS rate setting and with Multiple HX711's performing conversions simultaneously.
Tare function can also be performed without blocking the mcu.
 
Filtering and smoothing: "Moving average" method from a rolling data set combined with removal of high/low outliers is used for the retrieved value.

Selectable values in the config.h file:
- Moving average data set of 1, 2, 4, 8, 16, 32, 64 or 128 samples (default:16).
- Ignore high outlier; one sample is added to the data set, the peak high value of all samples in the data set is ignored (default:1)
- Ignore low outlier; one sample is added to the data set, the peak low value of all samples in the data set is ignored (default:1)
- Enable delay for writing to sck pin. This could be required for faster mcu's like the ESP32 (default: no delay)
- Disable interrupts when sck pin is high. This could be required to avoid "power down mode" if you have some other time consuming (>60µs) interrupt routines running (default: interrupts enabled)

Caution: using a high number of samples will smooth the output value nicely but will also increase settling time and start-up/tare time (but not response time). It will also eat some memory.

Important: The HX711 sample rate can be set to 10SPS or 80SPS (samples per second) by pulling pin 15 high (80SPS) or low (10SPS), ref HX711 data sheet.
On fabricated HX711 modules there is usually a solder jumper on the PCB for pin 15 high/low. The rate setting can be checked by measuring the voltage on pin 15.
ADC noise is worst on the 80SPS rate. Unless very quick settling time is required, 10SPS should be the best sample rate for most applications.

Start up and tare: from start-up/reset, the tare function seems to be more accurate if called after a "pre-warm-up" period running conversions continuously for a few seconds. See example files.

Hardware and ADC noise:
Wires between HX711 and load cell should be twisted and kept as short as possible.
Most available HX711 modules seems to follow the reference design, but be aware that some modules are poorly designed with under-sized capacitors, and noisy readings.
The Sparkfun module seems to differ from most other available modules as it has some additional components for noise reduction. 

To get started: Install the library from Arduino Library Manager. Begin with the Calibration.ino example file, then move on to the Read_1x_load_cell.ino example file.

If you need to keep the tare/zero-offset value after a device reboot, please see example Persistent_zero_offset.ino example file.

HX711_ADC Library Documentation
```
Initialization:
 begin(): Initializes the communication with the HX711 and sets the gain to 128 (default).
 begin(uint8_t gain): Initializes the communication with the HX711 and sets the gain (32, 64, or 128).
 Tare (Zero Point Calibration):
 tare(): Performs tare operation (blocking, waits until finished).
 tareNoDelay(): Initiates tare operation (non-blocking, continues in background).
 getTareStatus(): Returns true if the tare operation is complete.

Data Acquisition:
 update(): Reads a new weight sample (blocking, waits for conversion).
 dataWaitingAsync(): Checks if new weight data is available (non-blocking).
 updateAsync(): Reads new weight data if available (non-blocking, called after dataWaitingAsync).
 getData(): Returns the latest weight value (after applying calibration and filtering).

Calibration:
 setCalFactor(float cal): Sets the calibration factor for weight conversion (weight = raw data / calFactor).
 getCalFactor(): Returns the current calibration factor.
 getNewCalibration(float known_mass): Calculates and sets a new calibration factor based on a known mass.

Other Functions:
 setSamplesInUse(int samples): Sets the number of samples used for averaging and filtering (rounded down).
 getSamplesInUse(): Returns the current number of samples in use.
 resetSamplesIndex(): Resets the index for the dataset.
 refreshDataSet(): Refreshes the entire dataset with new conversions (blocking).
 getDataSetStatus(): Checks if the dataset is filled with conversions.
 getSignalTimeoutFlag(): Indicates if the HX711 communication timed out.
 setReverseOutput(): Reverses the output value (positive/negative).
 getTareOffset(): Gets the current tare offset (raw data value).
 setTareOffset(long newoffset): Sets a new tare offset (raw data value).
 powerUp(): Powers up the HX711 chip.
 powerDown(): Powers down the HX711 chip.
 getReadIndex(): Returns the current dataset read index (debugging).
 getConversionTime(): Returns the latest conversion time in milliseconds (debugging).
 getSPS(): Estimates the HX711 conversions per second (debugging).
 getTareTimeoutFlag(): Returns the tare operation timeout flag (debugging).
 disableTareTimeout(): Disables the tare operation timeout.
 getSettlingTime(): Calculates the estimated settling time based on conversion time and sample count (debugging).
```
