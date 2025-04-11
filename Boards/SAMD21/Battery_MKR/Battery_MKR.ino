/*
  MKR battery voltage measurement
 
  - MKR boards must be supplied by USB, or by a Vin of 5V (not more and 
    not less) or by a single cell 3.7V Li-Po or Li-Ion of 1000mAh or 
    more (the charging current is given and fixed, so never use 
    batteries with less capacity).

  - Most batteries you can purchase on-line have the polarity reversed 
    in comparison to the plug on the Arduino board. In-built battery 
    protection circuit stops the battery from working, just invert the 
    battery cables at the JST PH2 connector.

  - Standard so called Li-Po protection circuits integrated into many 
    packs do cutoff at around 2.4V, better is to cut the battery power 
    at 3.0V - 3.2V with a MOSFET circuit controlled by your Arduino.

  - Some MKR boards do not have an internal connection between the 
    battery and an analog input and thus ADC_BATTERY is not defined
    for them. For those boards you have to manually connect the 
    positive wire of the battery to a voltage divider (use the same 
    values from the MKR Zero schematics) and then measure the voltage 
    divider output with one of the analog inputs.
*/

void setup()
{               
  // Pin mode
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // "Display" battery voltage with flashes 
  // Note: voltage divider before the ADC_BATTERY analog 
  //       input is made by 1.2MΩ and 330kΩ resistors.
  //       adcVolt = battVolt * 1.2 / (1.2 + 0.33)
  float battVolt = analogRead(ADC_BATTERY) * 3.3 / 1023.0 / 1.2 * (1.2 + 0.33);
  int digit1 = (int)battVolt;
  int digit2 = (int)(10.0 * (battVolt - digit1));
  for (int i = 0 ; i < digit1 ; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  delay(1000);
  for (int i = 0 ; i < digit2 ; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
  }
  delay(5000);
}
