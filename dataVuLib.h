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

#include <Arduino.h>
#include <EEPROM.h>

// Verison number
#define DATAVULIB_VERSION "1.0"

// Define ATMega pins
#define PWM_LATCH   7
#define PWM_SDI     5
#define PWM_SCKI    6
#define PWM_PCLK    3
#define PWM_SDO     4
#define DAC         9
#define BTN1        10
#define BTN2        8
#define BTN3        21
#define BTN4        20

// Hard coded fast pin switches
#define SET_LATCH(state)    (state ? PORTD |= (1UL << 7) : PORTD &= ~(1UL << 7))
#define SET_SDI(state)      (state ? PORTD |= (1UL << 5) : PORTD &= ~(1UL << 5))
#define SET_SCKI(state)     (state ? PORTD |= (1UL << 6) : PORTD &= ~(1UL << 6))

// Define LT8500 command codes
#define UPDATE_PWM_CMD          0x00
#define ENABLE_PWM_CMD          0x30
#define DISABLE_PWM_CMD         0x40
#define TOGGLE_CORRECTION_CMD   0x70
#define UPDATE_CORRECTION_CMD   0x20
#define TOGGLE_PHASE_SHIFT_CMD  0x06

// Maximum allowable voltage
#define MAX_VOLTAGE 5

// Define converstion factors
#define V2DAC 51    // Convert 0-5V to eight bit integer

// Number of PWM channels (2x LT8500 chips)
#define PWM_CHANNEL_COUNT 96

// EEPROM addresses
#define CALIBRATION_ADDR 0     // Address for calibration data

///////////////////////////////////////////NO_DISPLAY//////////////////////////////////////////

#ifdef ARDUINO_NO_DISPLAY

#define DISPLAY_TYPE "No Display"
#define SYMBOL_COUNT 84
#define DIGIT_COUNT 0

// Define symbol numbering and map to PWM channels
const int SYMBOL_MAP[PWM_CHANNEL_COUNT] = {
    23, //PWM100
    33, //PWM101
    24, //PWM102
    35, //PWM103
    81, //PWM104
    80, //PWM105
    52, //PWM106
    77, //PWM107
    79, //PWM108
    53, //PWM109
    36, //PWM110
    83, //PWM111
    69, //PWM112
    75, //PWM113
    43, //PWM114
    30, //PWM115
    59, //PWM116
    -1, //PWM117
    -1, //PWM118
    74, //PWM119
    42, //PWM120
    31, //PWM121
    73, //PWM122
     8, //PWM123
    34, //PWM124
    64, //PWM125
    29, //PWM126
    70, //PWM127
    46, //PWM128
    32, //PWM129
    13, //PWM130
    -1, //PWM131
    28, //PWM132
    -1, //PWM133
    -1, //PWM134
     3, //PWM135
    -1, //PWM136
    -1, //PWM137
    -1, //PWM138
    -1, //PWM139
    45, //PWM140
    27, //PWM141
    -1, //PWM142
    -1, //PWM143
    -1, //PWM144
    67, //PWM145
    20, //PWM146
    49, //PWM147
    51, //PWM200
    72, //PWM201
    21, //PWM202
     5, //PWM203
    39, //PWM204
    55, //PWM205
    22, //PWM206
     1, //PWM207
    18, //PWM208
     4, //PWM209
    40, //PWM210
    50, //PWM211
    12, //PWM212
    17, //PWM213
    56, //PWM214
    38, //PWM215
    54, //PWM216
    15, //PWM217
    76, //PWM218
    19, //PWM219
    41, //PWM220
    37, //PWM221
     6, //PWM222
     9, //PWM223
     2, //PWM224
    44, //PWM225
    61, //PWM226
    47, //PWM227
    16, //PWM228
    48, //PWM229
    57, //PWM230
    14, //PWM231
    63, //PWM232
    65, //PWM233
     7, //PWM234
    71, //PWM235
    62, //PWM236
    66, //PWM237
    11, //PWM238
    78, //PWM239
    58, //PWM240
    26, //PWM241
    68, //PWM242
     0, //PWM243
    10, //PWM244
    25, //PWM245
    60, //PWM246
    82  //PWM247
};

#endif

///////////////////////////////////////////PD01002/4/6/8 - Normal View//////////////////////////////////////////

#ifdef ARDUINO_DATAVU_NORMAL

#define DISPLAY_TYPE "Normal"
#define SYMBOL_COUNT 61
#define DIGIT_COUNT 6

// Mapping symbol names to symbol numbers
#define A01 0
#define A02 1
#define A03 2
#define A04 3
#define N0A 4
#define N0B 5
#define N0C 6
#define N0D 7
#define N0E 8
#define N0F 9
#define N0G 10
#define N1A 11
#define N1B 12
#define N1C 13
#define N1D 14
#define N1E 15
#define N1F 16
#define N1G 17
#define N2A 18
#define N2B 19
#define N2C 20
#define N2D 21
#define N2E 22
#define N2F 23
#define N2G 24
#define N3A 25
#define N3B 26
#define N3C 27
#define N3D 28
#define N3E 29
#define N3F 30
#define N3G 31
#define N4A 32
#define N4B 33
#define N4C 34
#define N4D 35
#define N4E 36
#define N4F 37
#define N4G 38
#define N5A 39
#define N5B 40
#define N5C 41
#define N5D 42
#define N5E 43
#define N5F 44
#define N5G 45
#define D01 46
#define D02 47
#define D03 48
#define S01 49
#define S02 50
#define S03 51
#define S04 52
#define S05 53
#define S06 54
#define S07 55
#define S08 56
#define S09 57
#define S10 58
#define S11 59
#define S12 60
#define XXX -1 // Not connected to a symbol

// Define symbol numbering and map to PWM channels
const int SYMBOL_MAP[PWM_CHANNEL_COUNT] = {
    XXX, //PWM100
    A02, //PWM101
    XXX, //PWM102
    S09, //PWM103
    N4A, //PWM104
    N4G, //PWM105
    XXX, //PWM106
    XXX, //PWM107
    N1D, //PWM108
    A01, //PWM109
    XXX, //PWM110
    XXX, //PWM111
    N2E, //PWM112
    N3C, //PWM113
    N1B, //PWM114
    D01, //PWM115
    S12, //PWM116
    XXX, //PWM117
    XXX, //PWM118
    N4C, //PWM119
    XXX, //PWM120 
    XXX, //PWM121 
    N0B, //PWM122
    XXX, //PWM123
    N0C, //PWM124
    N4B, //PWM125
    N5B, //PWM126
    N3D, //PWM127
    N1G, //PWM128
    S04, //PWM129
    N1A, //PWM130
    XXX, //PWM131 
    N5C, //PWM132
    XXX, //PWM133
    XXX, //PWM134
    XXX, //PWM135
    XXX, //PWM136
    XXX, //PWM137
    XXX, //PWM138
    XXX, //PWM139
    S10, //PWM140
    N5F, //PWM141
    XXX, //PWM142
    XXX, //PWM143
    XXX, //PWM144
    S08, //PWM145
    N3A, //PWM146
    N2D, //PWM147
    A03, //PWM200
    N3E, //PWM201
    N2A, //PWM202
    S05, //PWM203
    N2C, //PWM204
    S07, //PWM205
    N1F, //PWM206
    N2B, //PWM207
    S01, //PWM208
    XXX, //PWM209
    XXX, //PWM210
    N3F, //PWM211
    XXX, //PWM212
    XXX, //PWM213
    N0D, //PWM214
    N1C, //PWM215
    S06, //PWM216
    XXX, //PWM217 
    N4F, //PWM218
    A04, //PWM219
    S03, //PWM220 
    XXX, //PWM221 S12 (repeated)
    N5A, //PWM222
    XXX, //PWM223
    D02, //PWM224
    N0G, //PWM225
    N0E, //PWM226
    XXX, //PWM227 S12 (repeated)
    N5E, //PWM228
    XXX, //PWM229 S12 (repeated)
    D03, //PWM230
    N0A, //PWM231
    N4D, //PWM232
    S02, //PWM233
    N5D, //PWM234
    N4E, //PWM235
    N1E, //PWM236
    XXX, //PWM237
    N2F, //PWM238
    N3G, //PWM239
    S11, //PWM240
    N5G, //PWM241
    XXX, //PWM242
    N3B, //PWM243
    N2G, //PWM244
    N0F, //PWM245
    XXX, //PWM246
    XXX  //PWM247
};

// Seven segment display mapping
const int SEVSEG_MAP[DIGIT_COUNT][7] = {
    
    // Digit 0
    {       // 7-Segment Map
        N0A,  // A
        N0B,  // B
        N0C,  // C
        N0D,  // D
        N0E,  // E
        N0F,  // F
        N0G   // G
    },
    
    // Digit 1
    {       // 7-Segment Map
        N1A,  // A
        N1B,  // B
        N1C,  // C
        N1D,  // D
        N1E,  // E
        N1F,  // F
        N1G   // G
    },
    
    // Digit 2
    {       // 7-Segment Map
        N2A,  // A
        N2B,  // B
        N2C,  // C
        N2D,  // D
        N2E,  // E
        N2F,  // F
        N2G   // G
    },
    
    // Digit 3
    {       // 7-Segment Map
        N3A,  // A
        N3B,  // B
        N3C,  // C
        N3D,  // D
        N3E,  // E
        N3F,  // F
        N3G   // G
    },
    
    // Digit 4
    {       // 7-Segment Map
        N4A,  // A
        N4B,  // B
        N4C,  // C
        N4D,  // D
        N4E,  // E
        N4F,  // F
        N4G   // G
    },
    
    // Digit 5
    {       // 7-Segment Map
        N5A,  // A
        N5B,  // B
        N5C,  // C
        N5D,  // D
        N5E,  // E  
        N5F,  // F
        N5G   // G
    }       
};

#endif

///////////////////////////////////////////PD01003/5/7/9 - Inverted View//////////////////////////////////////////

#ifdef ARDUINO_DATAVU_INVERTED

#define DISPLAY_TYPE "Inverted"
#define SYMBOL_COUNT 61
#define DIGIT_COUNT 6

// Mapping symbol names to symbol numbers
#define A01 0
#define A02 1
#define A03 2
#define A04 3
#define N0A 4
#define N0B 5
#define N0C 6
#define N0D 7
#define N0E 8
#define N0F 9
#define N0G 10
#define N1A 11
#define N1B 12
#define N1C 13
#define N1D 14
#define N1E 15
#define N1F 16
#define N1G 17
#define N2A 18
#define N2B 19
#define N2C 20
#define N2D 21
#define N2E 22
#define N2F 23
#define N2G 24
#define N3A 25
#define N3B 26
#define N3C 27
#define N3D 28
#define N3E 29
#define N3F 30
#define N3G 31
#define N4A 32
#define N4B 33
#define N4C 34
#define N4D 35
#define N4E 36
#define N4F 37
#define N4G 38
#define N5A 39
#define N5B 40
#define N5C 41
#define N5D 42
#define N5E 43
#define N5F 44
#define N5G 45
#define D01 46
#define D02 47
#define D03 48
#define S01 49
#define S02 50
#define S03 51
#define S04 52
#define S05 53
#define S06 54
#define S07 55
#define S08 56
#define S09 57
#define S10 58
#define S11 59
#define S12 60
#define XXX -1 // Not connected to a symbol

// Define symbol numbering and map to PWM channels
const int SYMBOL_MAP[PWM_CHANNEL_COUNT] = {
    N3F, //PWM100
    N0B, //PWM101
    XXX, //PWM102
    N5G, //PWM103
    XXX, //PWM104
    N2D, //PWM105
    XXX, //PWM106
    S12, //PWM107
    XXX, //PWM108
    S05, //PWM109
    XXX, //PWM110
    XXX, //PWM111
    N4B, //PWM112
    N3D, //PWM113
    XXX, //PWM114
    N3B, //PWM115
    N4F, //PWM116
    XXX, //PWM117
    XXX, //PWM118
    N2C, //PWM119
    S06, //PWM120
    N1B, //PWM121
    A02, //PWM122
    XXX, //PWM123 S12 (repeated)
    N5E, //PWM124
    N2E, //PWM125
    N1G, //PWM126
    N3C, //PWM127
    N5B, //PWM128
    XXX, //PWM129
    A03, //PWM130
    XXX, //PWM131
    N0D, //PWM132
    XXX, //PWM133
    XXX, //PWM134
    N0F, //PWM135
    XXX, //PWM136
    XXX, //PWM137
    XXX, //PWM138
    XXX, //PWM139
    N5D, //PWM140
    N0G, //PWM141
    XXX, //PWM142
    XXX, //PWM143
    XXX, //PWM144
    N5A, //PWM145
    S03, //PWM146
    N4G, //PWM147
    N1A, //PWM200
    N1F, //PWM201
    N2F, //PWM202
    A01, //PWM203
    N4C, //PWM204
    XXX, //PWM205
    N3E, //PWM206
    N2G, //PWM207
    N0E, //PWM208
    XXX, //PWM209
    S04, //PWM210
    XXX, //PWM211
    XXX, //PWM212
    S07, //PWM213
    N5C, //PWM214
    A04, //PWM215
    XXX, //PWM216
    XXX, //PWM217
    XXX, //PWM218 S12 (repeated)
    N1C, //PWM219
    N3A, //PWM220
    XXX, //PWM221
    S08, //PWM222
    N1D, //PWM223
    N3G, //PWM224
    N5F, //PWM225
    S01, //PWM226
    XXX, //PWM227
    N0C, //PWM228
    XXX, //PWM229
    N4D, //PWM230
    XXX, //PWM231
    D03, //PWM232
    S11, //PWM233
    S10, //PWM234
    N1E, //PWM235
    N4E, //PWM236
    N0A, //PWM237
    N2A, //PWM238
    D02, //PWM239
    S02, //PWM240
    S09, //PWM241
    N4A, //PWM242
    D01, //PWM243
    N2B, //PWM244
    XXX, //PWM245
    XXX, //PWM246
    XXX  //PWM247
};

// Seven segment display mapping
const int SEVSEG_MAP[DIGIT_COUNT][7] = {
    
    // Digit 0
    {       // 7-Segment Map
        N0A,  // A
        N0B,  // B
        N0C,  // C
        N0D,  // D
        N0E,  // E
        N0F,  // F
        N0G   // G
    },
    
    // Digit 1
    {       // 7-Segment Map
        N1A,  // A
        N1B,  // B
        N1C,  // C
        N1D,  // D
        N1E,  // E
        N1F,  // F
        N1G   // G
    },
    
    // Digit 2
    {       // 7-Segment Map
        N2A,  // A
        N2B,  // B
        N2C,  // C
        N2D,  // D
        N2E,  // E
        N2F,  // F
        N2G   // G
    },
    
    // Digit 3
    {       // 7-Segment Map
        N3A,  // A
        N3B,  // B
        N3C,  // C
        N3D,  // D
        N3E,  // E
        N3F,  // F
        N3G   // G
    },
    
    // Digit 4
    {       // 7-Segment Map
        N4A,  // A
        N4B,  // B
        N4C,  // C
        N4D,  // D
        N4E,  // E
        N4F,  // F
        N4G   // G
    },
    
    // Digit 5
    {       // 7-Segment Map
        N5A,  // A
        N5B,  // B
        N5C,  // C
        N5D,  // D
        N5E,  // E  
        N5F,  // F
        N5G   // G
    }       
};

#endif

// Only needed for devices with seven segment displays
#if DIGIT_COUNT > 0

//This is the combined array that contains all the segment configurations for many different characters and symbols. This is used in the updateDigit function.
const uint8_t CHARACTERARRAY[] PROGMEM = {
//    ABCDEFG  Segments      7-segment map:
    0b1111110, // 0   "0"          AAA
    0b0110000, // 1   "1"         F   B
    0b1101101, // 2   "2"         F   B
    0b1111001, // 3   "3"          GGG
    0b0110011, // 4   "4"         E   C
    0b1011011, // 5   "5"         E   C
    0b1011111, // 6   "6"          DDD
    0b1110000, // 7   "7"
    0b1111111, // 8   "8"
    0b1111011, // 9   "9"
    0b1110111, // 10  "A"
    0b0011111, // 11  "b"
    0b1001110, // 12  "C"
    0b0111101, // 13  "d"
    0b1001111, // 14  "E"
    0b1000111, // 15  "F"
    0b0000000, // 16  NO DISPLAY
    0b0000000, // 17  NO DISPLAY
    0b0000000, // 18  NO DISPLAY
    0b0000000, // 19  NO DISPLAY
    0b0000000, // 20  NO DISPLAY
    0b0000000, // 21  NO DISPLAY
    0b0000000, // 22  NO DISPLAY
    0b0000000, // 23  NO DISPLAY
    0b0000000, // 24  NO DISPLAY
    0b0000000, // 25  NO DISPLAY
    0b0000000, // 26  NO DISPLAY
    0b0000000, // 27  NO DISPLAY
    0b0000000, // 28  NO DISPLAY
    0b0000000, // 29  NO DISPLAY
    0b0000000, // 30  NO DISPLAY
    0b0000000, // 31  NO DISPLAY
    0b0000000, // 32  ' '
    0b0000000, // 33  '!'  NO DISPLAY
    0b0100010, // 34  '"'
    0b0000000, // 35  '#'  NO DISPLAY
    0b0000000, // 36  '$'  NO DISPLAY
    0b0000000, // 37  '%'  NO DISPLAY
    0b0000000, // 38  '&'  NO DISPLAY
    0b0100000, // 39  '''
    0b1001110, // 40  '('
    0b1111000, // 41  ')'
    0b0000000, // 42  '*'  NO DISPLAY
    0b0000000, // 43  '+'  NO DISPLAY
    0b0000100, // 44  ','
    0b0000001, // 45  '-'
    0b0000000, // 46  '.'  NO DISPLAY
    0b0000000, // 47  '/'  NO DISPLAY
    0b1111110, // 48  '0'
    0b0110000, // 49  '1'
    0b1101101, // 50  '2'
    0b1111001, // 51  '3'
    0b0110011, // 52  '4'
    0b1011011, // 53  '5'
    0b1011111, // 54  '6'
    0b1110000, // 55  '7'
    0b1111111, // 56  '8'
    0b1111011, // 57  '9'
    0b0000000, // 58  ':'  NO DISPLAY
    0b0000000, // 59  ';'  NO DISPLAY
    0b0000000, // 60  '<'  NO DISPLAY
    0b0000000, // 61  '='  NO DISPLAY
    0b0000000, // 62  '>'  NO DISPLAY
    0b0000000, // 63  '?'  NO DISPLAY
    0b0000000, // 64  '@'  NO DISPLAY
    0b1110111, // 65  'A'
    0b0011111, // 66  'b'
    0b1001110, // 67  'C'
    0b0111101, // 68  'd'
    0b1001111, // 69  'E'
    0b1000111, // 70  'F'
    0b1011110, // 71  'G'
    0b0110111, // 72  'H'
    0b0110000, // 73  'I'
    0b0111000, // 74  'J'
    0b0000000, // 75  'K'  NO DISPLAY
    0b0001110, // 76  'L'
    0b0000000, // 77  'M'  NO DISPLAY
    0b0010101, // 78  'n'
    0b1111110, // 79  'O'
    0b1100111, // 80  'P'
    0b1110011, // 81  'q'
    0b0000101, // 82  'r'
    0b1011011, // 83  'S'
    0b0001111, // 84  't'
    0b0111110, // 85  'U'
    0b0000000, // 86  'V'  NO DISPLAY
    0b0000000, // 87  'W'  NO DISPLAY
    0b0000000, // 88  'X'  NO DISPLAY
    0b0111011, // 89  'y'
    0b0000000, // 90  'Z'  NO DISPLAY
    0b1001110, // 91  '['
    0b0000000, // 92  '\'  NO DISPLAY
    0b1111000, // 93  ']'
    0b0000000, // 94  '^'  NO DISPLAY
    0b0001000, // 95  '_'
    0b0000010, // 96  '`'
    0b1110111, // 97  'a' SAME AS CAP
    0b0011111, // 98  'b' SAME AS CAP
    0b0001101, // 99  'c'
    0b0111101, // 100 'd' SAME AS CAP
    0b1101111, // 101 'e'
    0b1000111, // 102 'F' SAME AS CAP
    0b1011110, // 103 'G' SAME AS CAP
    0b0010111, // 104 'h'
    0b0010000, // 105 'i'
    0b0111000, // 106 'j' SAME AS CAP
    0b0000000, // 107 'k'  NO DISPLAY
    0b0110000, // 108 'l'
    0b0000000, // 109 'm'  NO DISPLAY
    0b0010101, // 110 'n' SAME AS CAP
    0b0011101, // 111 'o'
    0b1100111, // 112 'p' SAME AS CAP
    0b1110011, // 113 'q' SAME AS CAP
    0b0000101, // 114 'r' SAME AS CAP
    0b1011011, // 115 'S' SAME AS CAP
    0b0001111, // 116 't' SAME AS CAP
    0b0011100, // 117 'u'
    0b0000000, // 118 'b'  NO DISPLAY
    0b0000000, // 119 'w'  NO DISPLAY
    0b0000000, // 120 'x'  NO DISPLAY
    0b0000000, // 121 'y'  NO DISPLAY
    0b0000000, // 122 'z'  NO DISPLAY
    0b0000000, // 123 '0b'  NO DISPLAY
    0b0000000, // 124 '|'  NO DISPLAY
    0b0000000, // 125 ','  NO DISPLAY
    0b0000000, // 126 '~'  NO DISPLAY
    0b0000000, // 127 'DEL'  NO DISPLAY
};
#endif

// Logan class prototype
class DataVu
{
        // Flag for the state of calibration feature
        int calState;
        
    public:
    
        // Software frame buffer
        int frameBuf[SYMBOL_COUNT];
    
        // Member functions
        DataVu(void);
        void begin(void);
        int setVoltage(float);
        int updateFrame(int);
        int updateSymbol(int, int);
        void writeFrame();
        void setCal(bool);
        int writeCal(int*, bool save = false);
        void resetChips();
        int updateDigit(char, int, int);
        
    private:
        void write2Chips(int, int*);
};
