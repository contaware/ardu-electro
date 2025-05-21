/*
  Drive one common cathode 7seg display with the 74HC595 chip
  
  - VDD supply for the 74HC595 chip is 2V - 6V. It can drive a maximum 
    of 70mA total, per output that's around 8mA.

  - A segment/dot LED drops around 1.9V:
    * With a 5V MCU we have 3.1V across the resistor, with 8mA that 
      gives 3.1V / 8mA = 387.5Ω -> To be safe use 470Ω resistors for 
      each segment and the dot.
    * With a 3.3V MCU we have 1.4V across the resistor, with 8mA that 
      gives 1.4V / 8mA = 175Ω -> To be safe use 220Ω resistors for 
      each segment and the dot.
  
  - Connect 74HC595 pin 10 (RESET/MR) to VDD.
  
  - Connect 74HC595 pin 13 (OE) to GND.
  
  - Data is shifted on the LOW to HIGH transitions of the SHCP input.
  
  - The data in the shift register is transferred to the storage 
    register on a LOW to HIGH transition of the STCP input.

  - Segments and dot:

         A
        ---
    F  |   |  B
        -G-
    E  |   |  C
        --- . 
         D    DP
*/
const byte LATCH_PIN = 9;     // 74HC595 pin 12 (STCP)
const byte CLOCK_PIN = 10;    // 74HC595 pin 11 (SHCP)
const byte DATA_PIN = 8;      // 74HC595 pin 14 (DS)

// 74HC595 name: Q0(QA) Q1(QB) Q2(QC) Q3(QD) Q4(QE) Q5(QF) Q6(QG) Q7(QH)
// 74HC595 pin:   15      1      2      3      4      5      6      7
//                 |      |      |      |      |      |      |      | 
// Segment pin:    7      6      4      2      1      9     10      5
// Segment name:   A      B      C      D      E      F      G     DP
// (common cathode pins are 3 and 8)
byte SevenSegDigits[] = { 0b11111100,  // = 0
                          0b01100000,  // = 1
                          0b11011010,  // = 2
                          0b11110010,  // = 3
                          0b01100110,  // = 4
                          0b10110110,  // = 5
                          0b10111110,  // = 6
                          0b11100000,  // = 7
                          0b11111110,  // = 8
                          0b11100110   // = 9
                        };
                             
void setup()
{
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);
  digitalWrite(DATA_PIN, LOW);
}

void SevenSegWrite(int digit, bool dot)
{
  byte digitOut = 0; // all OFF

  // Digit
  if (digit >= 0 && digit <= 9)
    digitOut = SevenSegDigits[digit];

  // Dot
  if (dot)
    digitOut |= 0b00000001;

  // Write segments and the dot
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, digitOut);  
  digitalWrite(LATCH_PIN, HIGH);
}

void loop()
{
  // Count from 0..9
  for (int i = 0 ; i <= 9 ; i++)
  {
    SevenSegWrite(i, false);
    delay(500);
  }

  // Flash 3x the dot
  for (int i = 0 ; i <= 2 ; i++)
  {
    SevenSegWrite(-1, false);
    delay(200);
    SevenSegWrite(-1, true);
    delay(200);
  }
  delay(500);
}
