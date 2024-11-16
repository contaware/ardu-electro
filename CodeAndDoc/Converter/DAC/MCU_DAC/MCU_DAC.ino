/*
  Test Arduino boards which have a DAC (connect DAC pin to ADC pin)

  - UNO R4 (Minima and WiFi) have one 12-bit DAC (DAC=A0 pin).

  - Due has two 12-bit DACs (DAC0 and DAC1 pins) with a limited
    range: 0.55V (1/6 x 3.3V) - 2.75V (5/6 x 3.3V). 

  - SAMD51 has two 12-bit DACs (DAC0=A0 and DAC1=A1 pins).

  - SAMD21 has one 10-bit DAC (DAC0=A0 pin).

  - ESP32 has two 8-bit DACs.
    On ESP32:    first on GPIO25 and second on GPIO26.
    On ESP32-S2: first on GPIO17 and second on GPIO18.
    Note: DAC pins are not exposed on the Arduino Nano ESP32.
*/

// DAC pin
#if defined(ARDUINO_ARCH_ESP32)
#define DAC_PIN     25
#elif defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
#define DAC_PIN     DAC
#else
#define DAC_PIN     DAC0
#endif

// ADC pin
#define ADC_PIN     A3

// Global variables
int resolution;
unsigned long lastMillis;
int dacValue = -1; // -1 means not set

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.println("MCU DAC Test: type in upper window a DAC value and press ENTER");

  // Init resolution
#if defined(ARDUINO_ARCH_ESP32)
  resolution = 8;
  analogReadResolution(8);
#elif defined(ARDUINO_ARCH_SAMD) && !defined(__SAMD51__)
  resolution = 10;
  analogReadResolution(10);
  analogWriteResolution(10);
#else
  resolution = 12;
  analogReadResolution(12);
  analogWriteResolution(12);
#endif

  // Init millis
  lastMillis = millis();
}

int constrainToResolution(int value)
{
  int maxValue = (1 << resolution) - 1;
  return constrain(value, 0, maxValue);
}

void loop()
{
  // Get wanted value from serial monitor's input field and send it to the DAC
  if (Serial.available())
  {
    String msg;
    msg = Serial.readStringUntil('\n'); // function removes '\n' from serial buffer and does not return a '\n'
    msg.trim();                         // remove CR if terminal is sending one
    dacValue = constrainToResolution(msg.toInt());
#if defined(ARDUINO_ARCH_ESP32)
    // We must use dacWrite(), because analogWrite() 
    // outputs a PWM signal on all pins.
    dacWrite(DAC_PIN, dacValue);
#else
    analogWrite(DAC_PIN, dacValue);
#endif
  }

  // Serial print
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis > 1000)
  {
    lastMillis = currentMillis;
    if (dacValue >= 0)
    {
      Serial.print("DAC=");
      Serial.print(dacValue);
      Serial.print("  ");
    }
    Serial.print("ADC=");
    Serial.println(analogRead(ADC_PIN));
  }
}
