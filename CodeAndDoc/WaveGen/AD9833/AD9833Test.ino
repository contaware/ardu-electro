/*
  AD9833 waveform generator module with sine, triangle or square wave output
 
  - VDD supply is 2.3V - 5.5V.
    The internal digital section of the AD9833 is operated at 2.5V. An on-board 
    regulator steps-down the voltage applied at VDD to 2.5V. When the applied 
    voltage at the VDD pin of the AD9833 is less than or equal to 2.7V, the CAP/2.5V
    and VDD pins should be tied together, thus bypassing the on-board regulator.

  - It is recommended to add a 10uF capacitor in parallel with a 100nF capacitor 
    between VCC and GND.
  
  - Frequency range 0 to 12.5 MHz (with a 0.1Hz resolution).

  - The AD9833 creates sine waves by the use of a numerically controlled oscillator,
    phase modulator, sin look-up ROM and DAC. The output DAC is a high impedance 
    10-bit current source DAC. A built-in 200 ohm load resistor converts the current
    to a voltage that swings between ground and about +0.6V. At frequencies above 1MHz,
    the signal amplitude starts decreasing.

  - When the triangle waveform is selected, the sin look-up ROM is bypassed and the 
    output of the NCO is sent directly to the on-chip DAC which provides a 10-bit 
    triangular output. The triangle wave output will also be approximately 0.6Vpp up
    to 1MHz and then start to decrease at higher frequencies.
   
  - The square waveform is created by just sending the MSB (Most Significant Bit)
    of the DAC data to the output. Unlike the other two wave forms, the square
    wave output is a logic level and will swing the full range from ground to VDD.

  - Module connections:    
    REF = 25MHz reference clock from the on-board oscillator is output on this pin
    VCC = VDD supply (2.3V to 5.5V)
    GND = Ground
    DAT = SPI Bus Data pin  (MOSI)  note: MISO is not used since this is a write only device
    CLK = SPI Bus Clock pin (SCK)
    FNC = SPI Bus Load pin  (SS)    note: called FSYNC on the AD9833
    OUT = Output of the function generator
*/
#include <AD9833.h>       // download from https://github.com/Billwilliams1952/AD9833-Library-Arduino
 
const byte FNC_PIN = 10;  // can be any digital IO pin
float initFrequencyHz = 1000000.0;
AD9833 gen(FNC_PIN);      // defaults to 25MHz internal reference frequency

void DoSerial()
{
  char ch = toupper(Serial.read());
  switch (ch)
  {
    case 'F':
    {
      delay(5);  // give time for more characters to arrive
      byte index = 0;
      const byte READ_ARRAY_SIZE = 15;
      char readArray[READ_ARRAY_SIZE];
      memset(readArray, 0, sizeof(char) * READ_ARRAY_SIZE); // init to '\0'
      while (Serial.available())
      {
        readArray[index++] = Serial.read();
        if (index == (READ_ARRAY_SIZE - 1)) // leave last string array element '\0'
          break;
        else
          delay(5); // give time for more characters to arrive
      }
      unsigned long freqValue = strtoul(readArray, NULL, 10);
      if (freqValue > 0)
      {
        gen.SetFrequency(REG0, freqValue);
        Serial.print("New frequency: ");
        Serial.print(gen.GetActualProgrammedFrequency(REG0));
        Serial.println(" Hz");
      }
      break;
    }

    case 'S':
      gen.SetWaveform(REG0, SINE_WAVE);
      Serial.println("Sine Wave");
      break;

    case 'T':
      gen.SetWaveform(REG0, TRIANGLE_WAVE);
      Serial.println("Triangle Wave");
      break;

    case 'Q':
      gen.SetWaveform(REG0, SQUARE_WAVE);
      Serial.println("Square Wave");
      break;

    case 'H':
      gen.SetWaveform(REG0, HALF_SQUARE_WAVE);
      Serial.println("Half Square Wave");
      break;

    default:
      break;
  }
}

void PrintCmds()
{
  Serial.println("Enter command:");
  Serial.println("F xxxxx = Set frequency in Hz");
  Serial.println("S       = Set SINE waveform output");
  Serial.println("T       = Set TRIANGLE waveform output");
  Serial.println("Q       = Set SQUARE waveform output");
  Serial.println("H       = Set HALF SQUARE waveform output");
}

void setup()
{
    // This MUST be the first command after declaring the AD9833 object
    gen.Begin();

    /*
      - There are two register sets, REG0 and REG1, each one can be programmed for:
        Signal type: SINE_WAVE, TRIANGLE_WAVE, SQUARE_WAVE, and HALF_SQUARE_WAVE
        Frequency:   0 to 12.5 MHz
        Phase:       0 to 360 degress (this is only useful if it is 'relative' to some
                     other signal such as the phase difference between REG0 and REG1)
      - In ApplySignal, if phase is not given, it defaults to 0.
      - To change the signal, you can just call ApplySignal again with a new frequency
        and/or signal type.
      - Use gen.SetOutputSource(REG1) to change to REG1.
    */
    gen.ApplySignal(SINE_WAVE, REG0, initFrequencyHz);

    // Turn ON the output
    gen.EnableOutput(true);

    // Serial
    Serial.begin(9600);
    PrintCmds();
    Serial.print("Current frequency: ");
    Serial.print(gen.GetActualProgrammedFrequency(REG0));
    Serial.println(" Hz");
}
 
void loop()
{
  if (Serial.available())
    DoSerial();
}
