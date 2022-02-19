/*
  HC-SR04

  1. A 5 volt pulse of at least 10 us (10 microseconds) in duration is applied to the Trigger pin.
  2. The HC-SR04 responds by transmitting a burst of eight pulses at 40 KHz. This 8-pulse pattern 
     makes the "ultrasonic signature" from the device unique, allowing the receiver to discriminate
     between the transmitted pattern and the ultrasonic background noise.
  3. The eight ultrasonic pulses travel through the air away from the transmitter. Meanwhile the
     Echo pin goes high to start forming the beginning of the echo-back signal. If the pulse is not
     reflected back then the Echo signal will timeout after 38 ms and return low.
     This produces a 38 ms pulse that indicates no obstruction within the range of the sensor.
  4. If the pulse is reflected back the Echo pin goes low when the signal is received. This produces
     a pulse whose width varies between 150 us to 25 ms, depending upon the time it took for the
     signal to be received. The width of the received pulse is used to calculate the distance to the
     reflected object.

  https://dronebotworkshop.com/hc-sr04-ultrasonic-distance-sensor-arduino/
*/
#define TRIG_PIN    10
#define ECHO_PIN    13
 
float duration, distance;
 
void setup()
{
  Serial.begin(9600);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  delay(500);
}
 
void loop()
{
  // Write a pulse to the HC-SR04 Trigger Pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure the response from the HC-SR04 Echo Pin
  duration = pulseIn(ECHO_PIN, HIGH); // duration in microseconds
  
  // Determine distance from duration
  // (use 343 m/s as speed of sound)
  distance = (duration / 2) * 0.0343;

  // Note: we can improve accuracy by reading the temperature in °C and the relative humidity in % with the DHT11 and using this formula:
  // sound_speed = 331.4 + (0.606 * temp) + (0.0124 * hum); // humidity is a small factor
  
  // Send results to Serial Monitor
  Serial.print("Distance = ");
  if (distance >= 400 || distance <= 2)
    Serial.println("Out of range");
  else
  {
    Serial.print(distance);
    Serial.println(" cm");
  }
  
  delay(1000);
}
