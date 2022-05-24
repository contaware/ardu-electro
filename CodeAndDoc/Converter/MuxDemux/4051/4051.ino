/*
  4051 8-channels analog mux/demux module which can switch also negative analog signals

  - VCC supply is 2V to 6V.
  - VEE negative power supply is 0V to -6V (can be connected to ground shorting the pads on the module).
  - Attention VCC – VEE must not exceed 10V according to datasheet, but the chip on the module already
    burned at 8V difference -> it is working safe up to a difference of 6.6V.
  - Z is the input/output signal pin which must be between VEE and VCC.
  - E is the active low enable channels pin (it has a pull-down to GND).
  - The VEE, VCC, and GND (called GD on the left side) pins are connected to each other
    on both sides of the module and there is no need to supply power to both sides.
*/

// Mux digital control pins
const byte S0_PIN = 8;
const byte S1_PIN = 9;
const byte S2_PIN = 10;
const byte controlPin[] = {S0_PIN, S1_PIN, S2_PIN};

// Mux analog signal pin
const byte SIG_PIN = A0;

int readMux(byte channel)
{
  for (int i = 0; i < 3; i++)
    digitalWrite(controlPin[i], bitRead(channel, i));
  return analogRead(SIG_PIN);
}

void setup()
{
  Serial.begin(9600);

  for (int i = 0; i < 3; i++)
  {
    pinMode(controlPin[i], OUTPUT);
    digitalWrite(controlPin[i], LOW);
  }
  
  for (int i = 0; i < 8; i ++)
  {
    char result[6];
    sprintf(result, "C%02d  ", i);
    Serial.print(result);
  }
  Serial.println();
  for (int i = 0; i < 8; i ++)
    Serial.print("---- ");
  Serial.println();
  Serial.println();
}

void loop()
{
  for (int i = 0; i < 8; i ++)
  {
    char result[6];
    sprintf(result, "%04d ", readMux(i));
    Serial.print(result);
  }
  Serial.println();
  Serial.println();
  delay(1000);
}
