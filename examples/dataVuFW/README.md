# DataVu Development Kit Firmware
This Arduino script comes pre-installed on Plessey's DataVu Development Kit driver. It allows control of the driver through a series of UART commands or external buttons. It essentially wraps a UART interface around the *DataVuLib* library. 

The DataVu development kit driver board uses a common anode voltage to drive up to 84 different symbols on a display. The intensity of each symbol can then be uniquely controlled by pulse width modulating their cathode connection. This PWM is controlled by two LT8500 ICs, which allow twelve bit resolution and a six bit correction feature. This firmware limits this PWM resolution to 8 bits.

The UART interface will print a '*>*' command prompt. It will wait for a carriage return (*\r*) before processing the command. When using Arduino's IDE serial monitor set line ending to *Carrage return*. If the command was not recognised or there was an error with one of the inputs (e.g. out of range) then it will return a '*?*' character. If the command is run successfully it will print the command prompt on a newline.

The firmware also includes some push button interfaces. There are four buttons connected to the BTN1, BTN2, BTN3 and BTN4 pins. These are left floating on the driver board and need to be pull down externally. **Buttons may be triggered if these pins are not pull down**.

| Configurations	|Value
|-------------------|:-----:|
|Baud Rate			| 9600
|Data Bits			| 8	
|Stop Bits			| 1				
|Parity				| None

This sketch is available in the examples section of the Arduino IDE when the *dataVuLib* library is installed:
**File>Examples>DataVuLib>DataVuFW**

## UART API

###  Help
```cpp
	help
```
```cpp
	h
```
>Prints a list of available commands with brief descriptions.

<br> 

###  Set Voltage
```cpp
	v <voltage>
```
>Sets the LED anode voltage. The voltage can be set between 0 and 5V. The DAC has eight bit resolution which equates to a voltage resolution of 20mV. The specified voltage may not match the output voltage exactly. The output voltage depend on the board voltage which the firmware assumes to be 5V. For an accurate voltage a direct measurement on the driver board is needed.
>
>**Parameters:**
&nbsp;&nbsp;&nbsp;&nbsp;***voltage*** - The voltage (in Volts) between 0 and 5. The value can be a decimal number. 
>
<br> 

### Update Frame
```cpp
	ua <value>
```
>Updates all values in the software frame buffer. This will reset all the values in the software frame buffer with the specified value. 
>
>**Parameters:** <br> 
&nbsp;&nbsp;&nbsp;&nbsp;***value*** - The eight bit (0-255) PWM value for all the symbols. This parameter is an integer.

<br>

### Update Symbol

```cpp
	us <symbol> <value>
```
>Updates a single symbol's PWM value in the software frame buffer.
>
>**Parameters:** <br> 
&nbsp;&nbsp;&nbsp;&nbsp;***symbol*** - The symbol number for which the PWM value will be updated. <br> 
&nbsp;&nbsp;&nbsp;&nbsp;***value*** - The eight bit (0-255) PWM value for the specified symbol. This parameter is an integer.

<br>

### Update Whole Frame
```cpp
	u <value_1> <value_2> .... <value_n>
```
>Updates all values in the software frame buffer with unique values. Here *n* is the number of symbols on the display. The ordering of the values is the same as the symbol numbers in the datasheet.
>
>**Parameters:** <br> 
&nbsp;&nbsp;&nbsp;&nbsp;***value*** - The eight bit (0-255) PWM value for the symbol. This parameter is an integer.

<br>

### Update Digit

```cpp
	ud <c> <digit> <value>
```
>Updates a seven segment element in the software frame buffer.
>
>**Parameters:** <br> 
&nbsp;&nbsp;&nbsp;&nbsp;***c*** - The character to show on the seven segment element. Not all characters can be shown on a seven segment display. <br> 
&nbsp;&nbsp;&nbsp;&nbsp;***digit*** - The digit number specifies which seven segment element to update. <br> 
&nbsp;&nbsp;&nbsp;&nbsp;***value*** - The eight bit (0-255) PWM value for the seven segment element. This parameter is an integer.

<br>

### Write Frame

```cpp
	w
```
>Writes the software frame buffer to the display. This needs to be run for any of the frame buffer updates in the previous commands to take effect.

<br>

### Calibration On

```cpp
	cOn
```
>Turns the calibration feature on. The update frame command will need to be run after to see the effects.

<br>

### Calibration Off

```cpp
	cOff
```
>Turns the calibration feature off. The update frame command will need to be run after to see the effects.

<br>

### Set Calibration

```cpp
	c <cal_1> <cal_2> .... <cal_n>
```
>Updates the calibration data. Here *n* is the number of symbols on the display. Each symbol can have a unique PWM weightings which allows for display non-uniformity to be corrected for. Each symbol can be corrected by a weighting of 0.5x to 1.5x with six bit resolution. This calibration data will be saved to the EEPROM on the ATMega and reload when the driver is power cycled. 
>
>**Parameters:** <br> 
&nbsp;&nbsp;&nbsp;&nbsp;***cal*** - A six bit calibration integers. The ordering of these calibration values is identical to the write whole frame command


## Examples
Set the voltage to 2.8V.

	v 2.7

Update the whole frame buffer with the same value.

	ua 2
	
Write the frame buffer to the display. 

	w

The display should now light up. We can now write the character '*A*' to the first seven segment element with and PWM value of 255. The character '*A*' should appear brighter then the rest of the display.

	ud A 0 10
	w