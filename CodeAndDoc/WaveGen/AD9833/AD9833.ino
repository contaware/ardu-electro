/*
  AD9833 waveform generator module with sine, triangle or square wave output
 
  - VDD supply is 2.3V - 5.5V.
    The internal digital section of the AD9833 is operated at 2.5V. An 
    on-board regulator steps-down the voltage applied at VDD to 2.5V. 
    When the applied voltage at the VDD pin of the AD9833 is less than 
    or equal to 2.7V, the CAP/2.5V and VDD pins should be tied together, 
    thus bypassing the on-board regulator.

  - It is recommended to add a 10uF capacitor in parallel with a 100nF 
    capacitor between VCC and GND.
  
  - Frequency range 0 to 12.5 MHz (with a 0.1Hz resolution).

  - The AD9833 creates sine waves by the use of a numerically controlled 
    oscillator, phase modulator, sin look-up ROM and DAC. The output DAC 
    is a high impedance 10-bit current source DAC. A built-in 200Ω load 
    resistor converts the current to a voltage that swings between 
    ground and about +0.6V. At frequencies above 1MHz, the signal 
    amplitude starts decreasing.

  - When the triangle waveform is selected, the sin look-up ROM is 
    bypassed and the output of the NCO is sent directly to the on-chip 
    DAC which provides a 10-bit triangular output. The triangle wave 
    output will also be approximately 0.6Vpp up to 1MHz and then start 
    to decrease at higher frequencies.
   
  - The square waveform is created by just sending the MSB (Most 
    Significant Bit) of the DAC data to the output. Unlike the other two 
    wave forms, the square wave output is a logic level and will swing 
    the full range from ground to VDD.

  - Module connections:    
    REF = 25MHz reference clock from the on-board oscillator is output on this pin
    VCC = VDD supply (2.3V to 5.5V)
    GND = Ground
    DAT = SPI Bus Data pin  (MOSI)  note: MISO is not used since this is a write only device
    CLK = SPI Bus Clock pin (SCK)
    FNC = SPI Bus Load pin  (SS)    note: called FSYNC on the AD9833
    OUT = Output of the function generator
*/
#include <AD9833.h>       // https://github.com/RobTillaart/AD9833
 
const byte FNC_PIN = 10;  // can be any digital IO pin
float initFrequencyHz = 1000.0;
AD9833 gen(FNC_PIN);

void doSerialRead()
{
  String msg;
  msg = Serial.readStringUntil('\n'); // function removes '\n' from serial buffer and does not return a '\n'
  msg.trim();                         // remove CR if terminal is sending one
  if (msg.length() == 0)              // if just pressing ENTER
    return;

  switch (toupper(msg[0]))
  {
    case 'F':
    {
      msg.remove(0, 1);               // remove first char
      msg.trim();                     // trim whitespaces
      long freqValue = msg.toInt();
      if (freqValue > 0)
      {
        gen.setFrequency(freqValue);
        Serial.print("New frequency: ");
        Serial.print(gen.getFrequency());
        Serial.println(" Hz");
      }
      break;
    }

    case 'O':
      gen.setWave(AD9833_OFF);
      Serial.println("Output OFF");
      break;

    case 'S':
      gen.setWave(AD9833_SINE);
      Serial.println("Sine Wave");
      break;

    case 'T':
      gen.setWave(AD9833_TRIANGLE);
      Serial.println("Triangle Wave");
      break;

    case 'Q':
      gen.setWave(AD9833_SQUARE1);
      Serial.println("Square Wave");
      break;

    case 'H':
      gen.setWave(AD9833_SQUARE2);
      Serial.println("Half Square Wave");
      break;

    default:
      break;
  }
}

void printCmds()
{
  Serial.println("Type in upper window and press ENTER:");
  Serial.println("F xxxxx = Set frequency in Hz");
  Serial.println("O       = Output OFF");
  Serial.println("S       = Set SINE waveform output");
  Serial.println("T       = Set TRIANGLE waveform output");
  Serial.println("Q       = Set SQUARE waveform output");
  Serial.println("H       = Set HALF SQUARE waveform output");
}

void setup()
{
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  // Init AD9833
  SPI.begin();
  gen.begin();
  gen.setWave(AD9833_SINE);
  gen.setFrequency(initFrequencyHz);

  // Print Info
  Serial.print("AD9833_LIB_VERSION: ");
  Serial.println(AD9833_LIB_VERSION);
  printCmds();
  Serial.print("Current frequency: ");
  Serial.print(gen.getFrequency());
  Serial.println(" Hz");
}
 
void loop()
{
  if (Serial.available())
    doSerialRead();
}
