/*
  AMG8833 8x8 thermal camera sensor module
  
  - VDD supply is 3.3V - 5V.
  - The module has a 3.3V regulator and integrated level-shifters.
  - See the manual for the pixel disposition.
*/
#include <Adafruit_AMG88xx.h>

Adafruit_AMG88xx amg;

// INT pin from the sensor board goes to this pin on your microcontroller board
const byte INT_PIN = 3;

// Interrupt levels (in °C)
// Any reading on any pixel above TEMP_INT_HIGH or under TEMP_INT_LOW will trigger an interrupt
#define TEMP_INT_HIGH     30    // should be between 0 .. 80
#define TEMP_INT_LOW      0     // should be between 0 .. 80

// ISR
volatile bool interruptReceived = false;
void AMG88xx_ISR()
{
  interruptReceived = true;
}

/* 
We can tell which pixels triggered the interrupt by reading the
bits from interruptPixels[8] like:

         bit 0  bit 1  bit 2  bit 3  bit 4  bit 5  bit 6  bit 7
byte 0 |  01     02     03     04     05     06     07     08
byte 1 |  09     10     11     12     13     14     15     16
byte 2 |  17     18     19     20     21     22     23     24
byte 3 |  25     26     27     28     29     30     31     32
byte 4 |  33     34     35     36     37     38     39     40
byte 5 |  41     42     43     44     45     46     47     48
byte 6 |  49     50     51     52     53     54     55     56
byte 7 |  57     58     59     60     61     62     63     64
*/
byte interruptPixels[8];

void serialPrintBits(byte b)
{
  for (byte i = 0; i < 8; i++)
  {
    Serial.print(bitRead(b, i));
    Serial.print(" ");
  }
  Serial.println();
}

void setup()
{
  pinMode(INT_PIN, INPUT);
  
  Serial.begin(9600);
  Serial.println("AMG8833 interrupt test");
  Serial.println();

  // Default settings
  bool status = amg.begin();
  if (!status)
  {
    Serial.println("Could not find a valid AMG8833 sensor, check wiring!");
    while (true);
  }

  // Set interrupt levels
  amg.setInterruptLevels(TEMP_INT_HIGH, TEMP_INT_LOW);

  // Set to absolute value mode (I cannot find out how AMG88xx_DIFFERENCE works...)
  amg.setInterruptMode(AMG88xx_ABSOLUTE_VALUE);

  // Enable interrupts
  amg.enableInterrupt();

  // Attach to our Interrupt Service Routine (ISR)
  attachInterrupt(digitalPinToInterrupt(INT_PIN), AMG88xx_ISR, FALLING);
}

void loop()
{ 
  if (interruptReceived)
  {
    // Get which pixels triggered
    amg.getInterrupt(interruptPixels);

    // Print
    for (int i = 0; i < 8; i++)
      serialPrintBits(interruptPixels[i]);
    Serial.println();

    // Clear the interrupt so we can get the next one
    interruptReceived = false;
    amg.clearInterrupt();
  }
}
