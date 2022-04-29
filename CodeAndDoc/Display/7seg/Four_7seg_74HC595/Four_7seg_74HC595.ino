const byte LATCH_PIN = 9;     // 74HC595 pin 9 STCP
const byte CLOCK_PIN = 10;    // 74HC595 pin 10 SHCP
const byte DATA_PIN = 8;      // 74HC595 pin 8 DS
const byte DISPLAY0_PIN = 2;  // cathode of first 7-segments display
const byte DISPLAY1_PIN = 3;  // cathode of second 7-segments display
const byte DISPLAY2_PIN = 4;  // cathode of third 7-segments display
const byte DISPLAY3_PIN = 5;  // cathode of fourth 7-segments display

// Attention: connect 74HC595's pin 10 MR / SRCLR to 5V
//            connect 74HC595's pin 13 OE to 0V

// 74HC595 pin     Q0,Q1,Q2,Q3,Q4,Q5,Q6,Q7 
// Mapping to      a, b, c, d, e, f, g, dot of Seven-Segment LED
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
  pinMode(DISPLAY0_PIN, OUTPUT);
  pinMode(DISPLAY1_PIN, OUTPUT);
  pinMode(DISPLAY2_PIN, OUTPUT);
  pinMode(DISPLAY3_PIN, OUTPUT);
  digitalWrite(DISPLAY0_PIN, LOW);
  digitalWrite(DISPLAY1_PIN, LOW);
  digitalWrite(DISPLAY2_PIN, LOW);
  digitalWrite(DISPLAY3_PIN, LOW);
}

void SevenSegWrite(byte DisplayNum, byte Digit, bool Dot)
{
  // Turn all OFF
  pinMode(DISPLAY0_PIN, INPUT);
  pinMode(DISPLAY1_PIN, INPUT);
  pinMode(DISPLAY2_PIN, INPUT);
  pinMode(DISPLAY3_PIN, INPUT);

  // Init segments
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, Dot ? SevenSegDigits[Digit] | 0b00000001 : SevenSegDigits[Digit]);  
  digitalWrite(LATCH_PIN, HIGH);

  // Turn wanted display ON
  if (DisplayNum == 0)
  {
    pinMode(DISPLAY0_PIN, OUTPUT);
    pinMode(DISPLAY1_PIN, INPUT);
    pinMode(DISPLAY2_PIN, INPUT);
    pinMode(DISPLAY3_PIN, INPUT);
  }
  else if (DisplayNum == 1)
  {
    pinMode(DISPLAY0_PIN, INPUT);
    pinMode(DISPLAY1_PIN, OUTPUT);
    pinMode(DISPLAY2_PIN, INPUT);
    pinMode(DISPLAY3_PIN, INPUT);
  }
  else if (DisplayNum == 2)
  {
    pinMode(DISPLAY0_PIN, INPUT);
    pinMode(DISPLAY1_PIN, INPUT);
    pinMode(DISPLAY2_PIN, OUTPUT);
    pinMode(DISPLAY3_PIN, INPUT);
  }
  else
  {
    pinMode(DISPLAY0_PIN, INPUT);
    pinMode(DISPLAY1_PIN, INPUT);
    pinMode(DISPLAY2_PIN, INPUT);
    pinMode(DISPLAY3_PIN, OUTPUT);
  }
}

void loop()
{
  SevenSegWrite(0, 3, true);
  delayMicroseconds(1000);
  SevenSegWrite(1, 1, false);
  delayMicroseconds(1000);
  SevenSegWrite(2, 4, false);
  delayMicroseconds(1000);
  SevenSegWrite(3, 1, false);
  delayMicroseconds(1000);
}
