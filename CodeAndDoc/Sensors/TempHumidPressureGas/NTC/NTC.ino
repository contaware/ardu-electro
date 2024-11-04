const byte TEMP_PIN = A0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int tempReading = analogRead(TEMP_PIN);
  double tempK = log(10000.0 * ((1024.0 / tempReading - 1)));
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK ); // temp in Kelvin
  float tempC = tempK - 273.15; // convert Kelvin to Celsius
  Serial.print("Temp ");
  Serial.print(tempC);
  Serial.println("°C");
  delay(1000);
}
