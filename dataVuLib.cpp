/******************************************************************************
    This file is the main file for the Data-Vu evaluation kit Firmware 
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

#include "dataVuLib.h"

/**
    Class constructor
*/
DataVu::DataVu(void) {
    
    // Setup pins
    pinMode(PWM_SCKI,OUTPUT);
    pinMode(PWM_SDI, OUTPUT);
    pinMode(PWM_LATCH, OUTPUT);
    pinMode(PWM_PCLK, OUTPUT); 
    pinMode(PWM_SDO, INPUT);
    pinMode(DAC, OUTPUT);
    pinMode(BTN1, INPUT);
    pinMode(BTN2, INPUT);
    pinMode(BTN3, INPUT);
    pinMode(BTN4, INPUT);

    // Set pins initial states
    digitalWrite(PWM_SCKI, LOW);
    digitalWrite(PWM_SDI, LOW);
    digitalWrite(PWM_LATCH, LOW);
    digitalWrite(PWM_PCLK, LOW); 
    digitalWrite(DAC, LOW);
}

/**
    Initializes the DataVu object
*/
void DataVu::begin(void) {
    
    // Setup PWM_PCLK
    OCR2B = 0;
    OCR2A = 0;
    TCCR2A = 0;
    TCCR2B = 0;
    TCCR2A |= (1 << COM2B0) | (1 << WGM21);
    TCCR2B |= (1 << CS20);

    // Initialise frame buffer to zeros
    this->updateFrame(0);

    // Setup PWM chips
    this->resetChips();
    
    // Read and update calibration data from EEPROM
    int cal[SYMBOL_COUNT];
    EEPROM.get(CALIBRATION_ADDR, cal);
    this->writeCal(cal);

    // Setup PWM for the DAC
    OCR1A = 0;
    TCCR1A |= (1 << COM1A1) | (1 << WGM10);
    TCCR1B |= (1 << CS10) | (1 << WGM12); 
}

/**
    Sets the LED anode voltage
*/
int DataVu::setVoltage(float voltage) {
    
    // Check voltage is within range
    if (voltage > MAX_VOLTAGE || voltage < 0) {
        return 1;
    }
    
    // Convert voltage to eight bit value
    int value = int(voltage * V2DAC);
    
    // Set PWM duty cycle
    OCR1A = value;
    
    // Completed successfully
    return 0;
}

/**
    Update every symbol value in the frame buffer
*/
int DataVu::updateFrame(int val) {
    
    // Check for input errors
    if (val < 0 || val > 4095) {
        return 1;
    }
    
    // Update whole frame buffer
    for (int i = 0; i < SYMBOL_COUNT; i++) {
        this->frameBuf[i] = val;
    }
    
    // Completed successfully
    return 0;
}

/**
    Update specific sysmbol in the frame buffer
*/
int DataVu::updateSymbol(int symbol, int val) {
    
    // Check for input errors
    if (val < 0 || val > 4095) {
        return 1;
    }
    else if (symbol < 0 || symbol >= SYMBOL_COUNT) {
        return 2;
    }
    
    // Update single symbol in frame buffer.
    this->frameBuf[symbol] = val;
    
    // Completed successfully
    return 0;
}

/**
    Write current frame buffer to display
*/
void DataVu::writeFrame() {
    this->write2Chips(UPDATE_PWM_CMD, this->frameBuf);
}

/**
    Set the state of the calibration feature
*/
void DataVu::setCal(bool state) {

    if (state) {
        
        // Turn calibration feature ON
        if (not this->calState) {
            this->write2Chips(TOGGLE_CORRECTION_CMD, this->frameBuf);
            this->calState = state;
        }
    }
    else {
        
        // Turn calibration feature OFF
        if (this->calState) {
            this->write2Chips(TOGGLE_CORRECTION_CMD, this->frameBuf);
            this->calState = state;
        }
    }
}

/**
    Update calibration data with a save to EEPROM option
*/
int DataVu::writeCal(int cal[SYMBOL_COUNT], bool save) {
    
    // Check for input errors
    for (int i = 0; i++; i < SYMBOL_COUNT) {
        if (cal[i] < 0 || cal[i] > 63) {
            return 1;
        }
    }
    
    // Save latest calibration data to EEPROM
    if (save) {
        EEPROM.put(CALIBRATION_ADDR, cal);
    }
    
    // Reformat 6 bit values to 12 bit values
    int calTemp[SYMBOL_COUNT];
    for (int i = 0; i < SYMBOL_COUNT; i++) {
        calTemp[i] = cal[i] << 6;
    }
    
    // Write calibration values to PWM chips
    this->write2Chips(UPDATE_CORRECTION_CMD, calTemp);
    
    // Completed successfully
    return 0;
}

/**
    Reset PWM chips to default settings
*/
void DataVu::resetChips() {
    
    // Long reset pulse
    SET_LATCH(HIGH);
    delay(100);
    SET_LATCH(LOW);
    
    // Turn off correction feature which is enabled by default
    this->write2Chips(TOGGLE_CORRECTION_CMD, this->frameBuf);
    this->calState = false;
    
    // Turn on phase shift feature
    this->write2Chips(TOGGLE_PHASE_SHIFT_CMD, this->frameBuf);
    
    // Enable PWM output
    this->write2Chips(ENABLE_PWM_CMD, this->frameBuf);
}

/**
    Update seven segment display symbols in the frame buffer
*/
int DataVu::updateDigit(char c, int digit, int val) {
    
#if DIGIT_COUNT > 0

    // Check for input errors
    if (val < 0 || val > 4095) {
        return 1;
    }
    else if (digit < 0 || digit >= DIGIT_COUNT) {
        return 2;
    }
    
    // Update frame buffer with
    const uint8_t bitmap = pgm_read_byte(&CHARACTERARRAY[c]);
    for (int i = 0; i < 7; i++) {
        if (bitmap & (1<<i)) {
            this->frameBuf[SEVSEG_MAP[digit][6-i]] = val;
        }
        else {
            this->frameBuf[SEVSEG_MAP[digit][6-i]] = 0;
        }
    }
    
    // Completed successfully
    return 0;

#else
    // No seven segment display
    return 3;
#endif
}

/**
    Write frame to PWM chips 
*/
void DataVu::write2Chips(int cmd, int frame[SYMBOL_COUNT]) {
    
    // Define local data variable. This is the actual data which is sent to PWM chips.
    int data;
    
    // Define serial counter - MSB first. 
    int i = PWM_CHANNEL_COUNT - 1;
    
    // Write PWM values for chip 2
    while (i > 47) {
        
        // Map the PWM channel to symbol  number
        if (SYMBOL_MAP[i] < 0) {
            data = 0;
        }
        else {
            data = frame[SYMBOL_MAP[i]];
        }
        
        // Write data to PWM chip 2
        for (int j = 11; j >= 0; j--) {
            
            if (1 << j & data) {
                SET_SDI(HIGH);
            }
            else {
                SET_SDI(LOW);
            }
            SET_SCKI(HIGH);
            SET_SCKI(LOW);
        }
        i--;
    }
    
    // Write command for chip 2
    for (int i = 7; i >= 0; i--){
        if (1 << i & cmd) {
            SET_SDI(HIGH);
        }
        else {
            SET_SDI(LOW);
        }
        SET_SCKI(HIGH);
        SET_SCKI(LOW);
    }
    
    // Write PWM values for chip 1
    while (i >= 0) {
        
        // Map the PWM channel to symbol number
        if (SYMBOL_MAP[i] < 0) {
            data = 0;
        }
        else {
            data = frame[SYMBOL_MAP[i]];
        }
        
        // Write data to PWM chip 1
        for (int j = 11; j >= 0; j--) {
            if (1 << j & data) {
                SET_SDI(HIGH);
            }
            else {
                SET_SDI(LOW);
            }
            SET_SCKI(HIGH);
            SET_SCKI(LOW);
        }
        i--;
    }
    
    // Write command for chip 1
    for (int i = 7; i >= 0; i--){
        if (1 << i & cmd) {
            SET_SDI(HIGH);
        }
        else {
            SET_SDI(LOW);
        }
        SET_SCKI(HIGH);
        SET_SCKI(LOW);
    }
    
    // Set SDI low
    SET_SDI(LOW);

    // Latch data
    SET_LATCH(HIGH);
    SET_LATCH(LOW);
}
