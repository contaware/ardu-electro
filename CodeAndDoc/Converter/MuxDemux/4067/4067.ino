/*
  4067 16-channels analog mux/demux module

  - VCC supply is 2V to 6V.
  - EN is the active low enable channels pin (it has a 10k pull-down to GND).
*/

// Mux digital control pins
const byte S0_PIN = 8;
const byte S1_PIN = 9;
const byte S2_PIN = 10;
const byte S3_PIN = 11;
const byte controlPin[] = {S0_PIN, S1_PIN, S2_PIN, S3_PIN};

// Mux analog signal pin
const byte SIG_PIN = A0;

int readMux(byte channel)
{
  for (int i = 0; i < 4; i++)
    digitalWrite(controlPin[i], bitRead(channel, i));
  return analogRead(SIG_PIN);
}

void setup()
{
  Serial.begin(9600);

  for (int i = 0; i < 4; i++)
  {
    pinMode(controlPin[i], OUTPUT);
    digitalWrite(controlPin[i], LOW);
  }
  
  for (int i = 0; i < 16; i ++)
  {
    char result[6];
    sprintf(result, "C%02d  ", i);
    Serial.print(result);
  }
  Serial.println();
  for (int i = 0; i < 16; i ++)
    Serial.print("---- ");
  Serial.println();
  Serial.println();
}

void loop()
{
  for (int i = 0; i < 16; i ++)
  {
    char result[6];
    sprintf(result, "%04d ", readMux(i));
    Serial.print(result);
  }
  Serial.println();
  Serial.println();
  delay(1000);
}
