/*
  Guva-S12SD true UV measurement module 
  
  - Only use the adafruit original modules because the cheap purple 
    modules have a bad design:
    https://wolles-elektronikkiste.de/en/uv-sensors-and-sensor-modules
    
  - VDD supply is 2.7V – 5.5V.
  
  - Vout = 4.3 * Diode-Current in uA. So if the photocurrent is 1uA 
    (9 mW/cm^2), the output voltage is 4.3V.
    
  - UV index = Vout / 0.1V = 10 * Vout.
  
  - UV measurement range: 240nm - 370nm (which covers UVB and most of 
    UVA spectrum).
    
  - You can't just fix it outside like a thermometer because the 
    measured values depend on the angle of incidence. You have to 
    position the sensor towards the sun!
*/

const byte UV_PIN = A0;

void setup()
{
  // Init Serial
  Serial.begin(9600);
}

void loop()
{
  int analogSignal = analogRead(UV_PIN);
  float voltage = analogSignal / 1023.0 * 5.0;
  float uvIndex = voltage / 0.1;
  Serial.print("Signal: "); Serial.println(analogSignal);
  Serial.print("Volt: "); Serial.println(voltage);
  Serial.print("UV-Index: "); Serial.println(uvIndex);
  Serial.println("------------------------------");
  
  delay(1000);
}
