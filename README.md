
# DataVu Driver
DataVuLib is an Arduino c++ library designed for Plessey Semiconductor's DataVu development kit. It deals with the operation of the underlying hardware to allow quick development of custom firmware. The DataVu development kit driver board is based around a ATMega328. The DataVu development kit driver has a USB interface and comes pre-installed with a boot-loader to allow easy integration with the Arduino IDE. The driver board also come with DataVuFW Firmware installed on it which can be found in the examples - [here](https://github.com/Plessey/DataVuLib/tree/master/examples/dataVuFW).

The DataVu development kit driver board uses a common anode voltage to drive up to 84 different symbols on a display. The intensity of each symbol can then be uniquely controlled by pulse width modulating their cathode connection. This PWM is controlled by two LT8500 ICs, which allow twelve bit resolution and a six bit correction feature. This library is designed to work with the accompanying board definition files - found [here](https://github.com/Plessey/DataVuBoard). Since different displays have different numbers of symbols, symbol mappings and seven segment displays, the compilation of this library depends on the specific board chosen.



## Installation
1) Click **Clone or Download** button on the *DataVuLib* Github page and download a .ZIP archive of the library. Rename library if required
2) Open Arduino IDE (1.6 or later). 
3) Select *Sketch>Include Library>Add .ZIP Library*. Select the library archive downloaded in part 1.

## Example Useage
```cpp
#include <dataVuLib.h>

// Create dataVu object
DataVu dataVu;

void setup() {

    // Initialize object
    dataVu.begin();
    
    // Set voltage
    dataVu.setVoltage(2.7);

    // Turn whole display on for five seconds at low brightness
    dataVu.updateFrame(5);
    dataVu.writeFrame();
    delay(5000);
    dataVu.updateFrame(0);
    dataVu.writeFrame();
}

void loop() {

    // Count to 100 seconds
    int seconds = 0;
    for (int i = 0; i < 100; i++) {

        // Calculate digits
        int D1 = i % 10;
        int D2 = i / 10;

        // Clear frame buffer
        dataVu.updateFrame(0);

        // Write digits to frame buffer
        dataVu.updateDigit(D1, 0, 5);
        dataVu.updateDigit(D2, 1, 5);

        // Update display and wait for one second
        dataVu.writeFrame();
        delay(1000);
    }
}
```

## DataVu Class Reference
The ```DataVu``` class is the heart of **DataVuLib**. It abstracts the lower level hardware control to set of easy to use member functions. A ```DataVu``` object contains a software frame buffer. This can be modified by using the functions ```DataVu::updateFrame```, ```DataVu::updateSymbol``` and ```DataVu::updateDigit```. For these updates to be displayed the software frame buffer then needs to be written to the PWM chips using ```DataVu::writeFrame```. The software frame buffer can be edited directly by modifying ```DataVu.frameBuf[SYMBOL_COUNT]```. 

The calibration feature allow a unique correction weighting to be applied to every symbols PWM. This can then be saved in the ATMega328's EEPROM which is then loaded when the ```DataVu``` object is initialised.  

The symbol mapping in the software frame buffer follows the symbol numbering in the display datasheet. The calibration mapping is also identical. 

<br> 

```cpp
	DataVu::DataVu(void)
```
>Class constructor. Sets the pin modes and initialises their initial states.


<br>

```cpp
	void DataVu::begin()
```
>Initializes the DataVu object. This function configures the PWM chips, starts the PWM clock and configures the DAC pin.

<br>

```cpp
	int DataVu::setVoltage(float voltage)
```
>Sets the LED anode voltage. The voltage can be set between 0 and 5V. The DAC has eight bit resolution which equates to a voltage resolution of 20mV. The specified voltage may not match the output voltage exactly as it depends on the board voltage and display current. For an accurate voltage a direct measurement on the driver board is needed.
>
>**Parameters:** <br>
&nbsp;&nbsp;&nbsp;&nbsp;***voltage*** - The voltage (in Volts) between 0 and 5. The value can be a decimal number. 
>
>**Returns:** <br>
&nbsp;&nbsp;&nbsp;&nbsp;***0*** - Function completed without errors <br>
&nbsp;&nbsp;&nbsp;&nbsp;***1*** - Voltage was out of range

<br>

```cpp
	int DataVu::updateFrame(int val)
```
>Updates all values in the software frame buffer. This will reset all the values in the software frame buffer with the specified value.
>
>**Parameters:** <br>
&nbsp;&nbsp;&nbsp;&nbsp;***val*** - The twelve bit PWM value for all the symbols.
>
>**Returns:** <br>
&nbsp;&nbsp;&nbsp;&nbsp;***0*** - Function completed without errors <br>
&nbsp;&nbsp;&nbsp;&nbsp;***1*** - PWM value was out of range

<br>

```cpp
	int DataVu::updateSymbol(int symbol, int val)
```
>Updates a single symbol PWM value in the software frame buffer.
>
>**Parameters:** <br>
&nbsp;&nbsp;&nbsp;&nbsp;***symbol*** - The symbol number for which the PWM value will be updated <br>
&nbsp;&nbsp;&nbsp;&nbsp;***val*** - The twelve bit PWM value for the specified symbol.
>
>**Returns:** <br>
&nbsp;&nbsp;&nbsp;&nbsp;***0*** - Function completed without errors <br>
&nbsp;&nbsp;&nbsp;&nbsp;***1*** - PWM value was out of range

<br>

```cpp
	void DataVu::writeFrame()
```
>Writes the current software frame buffer to the PWM chips. This will update the display with the latest symbols PWM values.

<br>

```cpp
	void DataVu::setCal(bool state)
```
>Sets the state of the calibration feature. The calibration feature allows the luminance uniformity of the display to be balanced. When turned on each symbol will have a 0.5x to 1.5x weighting applied to its PWM value. Please see the LT8500 datasheet for more information - [link](https://www.analog.com/en/products/lt8500.html). Note: Run `writeFrame` for new calibration state to take effect. Calibration state is turned off by default.
>
>**Parameters:** <br>
&nbsp;&nbsp;&nbsp;&nbsp;***state*** - Setting this to `true` will turn on the calibration feature and `false` will turn it off.
>

<br>

```cpp
	int DataVu::writeCal(int cal[SYMBOL_COUNT], bool save = false)
```
>Updates the calibration data. The symbol PWM weightings are an array of six bit integers. The calibration data can be saved to the EEPROM on the ATMega. This calibration data is read and loaded back into the PWM chips when the DataVu class is `DataVu::begin()` is called.
>
>**Parameters:** <br>
&nbsp;&nbsp;&nbsp;&nbsp;***cal*** - An array of six bit calibration integers. The ordering of these calibration values is identical to the `frameBuf[SEGMENT_COUNT]`. <br>
&nbsp;&nbsp;&nbsp;&nbsp;***save*** - This is an optional argument. Setting this to `true` will save the calibration data to the EEPROM.
>
>**Returns:** <br>
&nbsp;&nbsp;&nbsp;&nbsp;***0*** - Returns with no errors <br>
&nbsp;&nbsp;&nbsp;&nbsp;***1*** - One or more calibration values are out of range.

<br>



```cpp
	void DataVu::resetChips()
```
>Resets the PWM chips to their defaults configuration.

<br>



<br>

```cpp
	int DataVu::updateDigit(char c, int digit, int val)
```
>Updates the software frame buffer for a seven segment portions of the display. This function uses a mapping to simplify updating a seven segment element.
>
>**Parameters:** <br>
&nbsp;&nbsp;&nbsp;&nbsp;***c*** - The character to be shown on the seven segment digit. This can be integers 0 to 9 or certain letters which can be displayed on seven segment display. Please see `CHARACTERARRAY[]` in *DataVuLib.h* for a full list of available characters. <br>
&nbsp;&nbsp;&nbsp;&nbsp;***digit*** - The seven segment digit number. Please see datasheet for the specific display used <br>
&nbsp;&nbsp;&nbsp;&nbsp;***val*** - The PWM value of the symbols in the seven segment digit. 
>
>**Returns:** <br>
&nbsp;&nbsp;&nbsp;&nbsp;***0*** - Function completed without errors <br>
&nbsp;&nbsp;&nbsp;&nbsp;***1*** - The PWM value is out of range <br>
&nbsp;&nbsp;&nbsp;&nbsp;***2*** - The digit value is out of range <br>
&nbsp;&nbsp;&nbsp;&nbsp;***3*** - Selected display has no seven segment elements

<br>

## Pre-processor Definitions

| Pre-Processor Definitions  |          Description				|   
|---------------------------|:---------------------------------|
| SYMBOL_COUNT 				| The number of symbols that the selected display has.  					|
| CALIBRATION_ADDR 			| The EEPROM address that the calibration data is saved and loaded from					|
| DIGIT_COUNT 				| The number of seven segment display elements the particular display has. 					|
| A01, N2F, S04, etc	| Each symbol has a symbol number used in the software frame buffer mapping. The symbol ID can be found in the display datasheet.  					|
|PWM_SDI, PWM_SCKI, PWM_PCLK, PWM_SDO DAC, BTN1/2/3/4     | Pin numbering. See driver board schematic. **Arduino pin mapping (e.g PB0) does not currently work**
