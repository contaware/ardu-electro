/*
  Serial Ports
  
  Some boards have several Serial Ports:
  - Serial is the main port for debug messages.
  - SerialUSB is the native USB port, also for debug messages.
  - Serial1 is the first additional hardware serial port.
  - Serial2 is another hardware serial port.
  - Serial3 is a third hardware serial port.
*/

// Test the various Serial ports by uncommenting them
#define MySerial        Serial
//#define MySerialUSB     SerialUSB
#define MySerial1       Serial1
//#define MySerial2       Serial2
//#define MySerial3       Serial3

void setup()
{
  // Init LED
  pinMode(LED_BUILTIN, OUTPUT);

  // Init Serial(s)
#ifdef MySerial
  MySerial.begin(9600);
#endif
#ifdef MySerialUSB
  MySerialUSB.begin(9600);
#endif
#ifdef MySerial1
  MySerial1.begin(9600);
#endif
#ifdef MySerial2
  MySerial2.begin(9600);
#endif
#ifdef MySerial3
  MySerial3.begin(9600);
#endif
}

void loop()
{
  // Count-up
  static unsigned long count = 0;
#ifdef MySerial
  MySerial.print("Serial: ");
  MySerial.println(count);
#endif
#ifdef MySerialUSB
  MySerialUSB.print("SerialUSB: ");
  MySerialUSB.println(count);
#endif
#ifdef MySerial1
  MySerial1.print("Serial1: ");
  MySerial1.println(count);
#endif
#ifdef MySerial2
  MySerial2.print("Serial2: ");
  MySerial2.println(count);
#endif
#ifdef MySerial3
  MySerial3.print("Serial3: ");
  MySerial3.println(count);
#endif
  count++;

  // Blink
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
