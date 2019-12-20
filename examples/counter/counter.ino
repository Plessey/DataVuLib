/******************************************************************************
    This file an example ro the Data-Vu evaluation kit library 
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