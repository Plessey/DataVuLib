/******************************************************************************
    This file is the header file for the Data-Vu evaluation kit library 
    created by Plessey Semiconductors.
    
    Copyright (C) 2019  <Gethn Pickard>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
******************************************************************************/

// Version Number
#define DATAVUFW_VERSION "1.0"

// Include libraries
#include <dataVuLib.h>
#include "Cmd.h"

// Help command string
const char COMMAND_LIST[] PROGMEM = {
    "\
                              DataVu Firmware Command List: \n\r\
    help, h                                             Prints a list of commands and descriptions\n\r\
    v <voltage>                                         Sets the volatge (0-5V) \n\r\
    ua <value>                                          Updates the whole frame buffer with value (0-255) \n\r\
    us <symbol> <value>                                 Updates symbol with value (0-255) \n\r\
    u <value_1> <value_2> ... <value_SYMBOL_COUNT>      Updates the whole frame buffer with the specified values \n\r\
    ud <c> <digit> <value>                              Updates a seven segment digit with the character c and PWM value. \n\r\
    w                                                   Write the software frame buffer to the PWM chips\n\r\
    cOn                                                 Turns on the calibration mode \n\r\
    cOff                                                Turns off the calibration mode \n\r\
    c <cal_1> <cal_2> ... <cal_SYMBOL_COUNT>            Write the calibration values and save them to EEPROM \n\r\
"};

// Create DataVu object
DataVu dataVu;

// Push button configurations
#define VOLTAGE 2.7     
#define DEBOUNCE_TIME 200
#define HOLD_TIME 1000
#define VALUE_COUNT 10

// Initialize button variables
volatile int counter = 0;
volatile bool state = false;
volatile long lastTime = 0;
volatile long currTime = 0;
int values[VALUE_COUNT] = {1, 5, 10, 20, 50, 100, 250, 500, 2000, 4095}; 
volatile int index = 0;
volatile bool inc_flag = false;

// Increments the counter and write new value to display
void incrementCounter() {

    // Check if increment function is already running
    if (inc_flag) {
        return;
    }

    // Set inc_flag
    inc_flag = true;

    // Reset frame buffer
    dataVu.updateFrame(0);

    // Calculate digits
    int D1 = counter % 10;
    int D2 = (counter / 10) % 10;
    int D3 = (counter / 100) % 10;
    int D4 = counter / 1000;

    // Write digits to frame buffer
    dataVu.updateDigit(D1, 0, values[index]);
    dataVu.updateDigit(D2, 1, values[index]);
    dataVu.updateDigit(D3, 2, values[index]);
    dataVu.updateDigit(D4, 3, values[index]);

    // Write frame buffer to PWM chips
    dataVu.writeFrame();

    // Increment counter
    if (counter == 9999) {
        counter = 0;
    }
    else {
        counter++;
    }

    // Unset inc_flag
    inc_flag = false;
}

// Pin change interrupt function - For BTN1,2,3
ISR(PCINT0_vect) {

    // Store current time
    currTime = millis();

    // Check if debounce time has elapsed since last interrupt
    if ((currTime - lastTime) > DEBOUNCE_TIME) {

        // Decrease brightness
        if (PINB & _BV(PB0)) {
            if (index != 0) {
                index--;
                dataVu.updateFrame(values[index]);
                dataVu.writeFrame();
            }
        }

        // Increase brightness
        else if (PINB & _BV(PB2)) {
            if (index != (VALUE_COUNT - 1)) {
                index++;
                dataVu.updateFrame(values[index]);
                dataVu.writeFrame();
            }
        }

        // Increment counter
        else if (PINB & _BV(PB7)) {
            incrementCounter();
        }
    }
    lastTime = currTime;
}

// Turns display on and off - BTN4
void power() {

    // Store current time
    currTime = millis();

    // Check if debounce time has elapsed since last interrupt
    if ((currTime - lastTime) > DEBOUNCE_TIME) {

        // Turn display off
        if (state) {
            dataVu.setVoltage(0);
            state = false;
        }

        // Turn display on and reset variables
        else {
              counter = 0;
              index = 0;
              dataVu.updateFrame(values[index]);
              dataVu.writeFrame();
              dataVu.setVoltage(VOLTAGE);
              state = true;
        }
    }
    lastTime = currTime;
}

void setup() {

    // Initialize dataVu object
    dataVu.begin();

    // Initialize UART CLI
    cmdInit(9600);

    // Add commands to CLI
    cmdAdd("help", cli_help);
    cmdAdd("h", cli_help);
    cmdAdd("v", cli_v);
    cmdAdd("ua", cli_ua);
    cmdAdd("us", cli_us);
    cmdAdd("u", cli_u);
    cmdAdd("ud", cli_ud);
    cmdAdd("w", cli_w);
    cmdAdd("cOn", cli_cOn);
    cmdAdd("cOff", cli_cOff);
    cmdAdd("c", cli_c);

    // Setup pin change interrupts
    cli();
    PCICR |= 0b00000001; // Turn on pin change interrupt on port B
    PCMSK0 |= 0b00000001; // Interrupt on pin PB0
    PCMSK0 |= 0b00000100; // Interrupt on pin PB2
    PCMSK0 |= 0b10000000; // Interrupt on pin PB7
    sei();

    // Attach external interrupt
    attachInterrupt(digitalPinToInterrupt(PD2), power, RISING);
}

void loop() {
    // CLI loop
    cmdPoll();

    // Check for button hold condition
    if (PINB & _BV(PB7)) {
        if (millis() - lastTime > HOLD_TIME) {
            incrementCounter();
        }
    }
}

// Prints a manual page showing the command set
int cli_help(int arg_cnt, char **args){

    // Print versions numbers
    Serial.println();
    Serial.println ("DataVuFW v"  DATAVUFW_VERSION);
    Serial.println ("DataVuLib v"  DATAVULIB_VERSION ", " DISPLAY_TYPE);

    // Read COMMAND_LIST from memory and prints
    for (int i = 0; i < strlen_P(COMMAND_LIST); i++) {
        char c = pgm_read_byte_near(COMMAND_LIST + i);
        Serial.print(c);
    };
    return 0;
}

// Sets the anode voltage for the display
int cli_v(int arg_cnt, char **args){

    // Check voltage range
    float voltage = atof(args[1]);
    if (voltage < 0 || voltage > MAX_VOLTAGE) {
        return 1;
    }
    else {
        dataVu.setVoltage(voltage);
    }
    return 0;
}

// Sets all the symbol's duty cyles with a common value
int cli_ua(int arg_cnt, char **args){

    // Check value range
    int value = atoi(args[1])*16;
    if (value < 0 || value > 4095) {
        return 1;
    }

    // Write values to frame buffer
    else {
        dataVu.updateFrame(value);
    }
    return 0;
}

// Sets a single symbol's PWM duty cycle 
int cli_us(int arg_cnt, char **args){

    // Check symbol and value range
    int symbol = atoi(args[1]);
    int value = atoi(args[2])*16;
    if (symbol < 0 || symbol >= SYMBOL_COUNT) {
        return 1;
    }
    else if (value < 0 || value > 4095) {
        return 1;
    }

    // Write value to frame buffer
    else {
        dataVu.updateSymbol(symbol, value);
    }
    return 0;
}

// Sets every symbol's PWM duty cycle with a unique value
int cli_u(int arg_cnt, char **args){

    // Check number of arguments
    if (arg_cnt != SYMBOL_COUNT + 1) {
        return 1;
    }

    // Loop over arguments  
    for (int i=0; i < SYMBOL_COUNT; i++) {
        int value = atoi(args[i+1]) * 16;

        // Check values are in range
        if (value < 0 || value > 4095) {
          return 1;
        }

        // Update frame buffer
        dataVu.frameBuf[i] = value;
    }
    return 0;
}

// Writes a digit to a seven segment element
int cli_ud(int arg_cnt, char **args){

    // Check character input 
    char c;
    if (strlen(args[1]) > 1) {
        return 1;
    }
    else {
        c = args[1][0];
        if (int(c) > 127) {
            return 1;
        }
    }

    // Check symbol number and value are in range
    int digit = atoi(args[2]);
    int value = atof(args[3]) * 16;
    if (digit < 0 || digit >= DIGIT_COUNT) {
        return 1;
    }
    else if (value < 0 || value > 4095) {
        return 1;
    }

    // Write digit to software buffer
    if (dataVu.updateDigit(c, digit, value)) {
        return 1; // No seven segment elements in chosen display.
    }
    return 0;
}


// Updates the display
int cli_w(int arg_cnt, char **args){
    dataVu.writeFrame();  
    return 0;
}

// Turns the calibration feature on
int cli_cOn(int arg_cnt, char **args){
    dataVu.setCal(true);
    return 0;
}

// Turns the calibration feature off
int cli_cOff(int arg_cnt, char **args){
    dataVu.setCal(false);
    return 0;
}

// Sets the calibration values 
int cli_c(int arg_cnt, char **args){

    // Check number of arguments
    if (arg_cnt != SYMBOL_COUNT + 1) {
        return 1;
  }

    // Create cal array
    int cal[SYMBOL_COUNT];

    // Loop over arguments  
    for (int i=0; i < SYMBOL_COUNT; i++) {
    int calTemp = atoi(args[i+1]);

        // Check values are in range
        if (calTemp < 0 || calTemp > 63) {
            return 1;
        }
        else {
            cal[i] = calTemp;
        }
    }

    // Update calibration
    dataVu.writeCal(cal, true);
    
    return 0;
}