/*
  Serial Ports
  
  Some boards have several Serial Ports:
  - Serial is the port used for debug prints.
    Note: Serial is sometimes a hardware serial and other times a serial 
    over USB. Additionally it may be an alias for one of the following ports.
  - SerialUSB is sometimes defined for the serial communication over USB.
  - Serial0 is the hardware serial port 0 for ESP32.
  - Serial1 is the hardware serial port 1.
  - Serial2 is the hardware serial port 2.
  - Serial3 is the hardware serial port 3.
*/

// Test the various Serial ports by uncommenting them
#define MySerial        Serial
//#define MySerialUSB     SerialUSB
//#define MySerial0       Serial0
#define MySerial1       Serial1
//#define MySerial2       Serial2
//#define MySerial3       Serial3

// For ESP32 we define the GPIOs/pins of Serial1 & Serial2
#define MY_RX1          21  // D10 or 21
#define MY_TX1          18  // D9  or 18
#define MY_RX2          17  // D8  or 17
#define MY_TX2          10  // D7  or 10

void setup()
{
#ifdef MySerial
  MySerial.begin(9600);
#endif
#ifdef MySerialUSB
  MySerialUSB.begin(9600);
#endif
#ifdef MySerial0
  MySerial0.begin(9600);
#endif
#ifdef MySerial1
  #if defined(ARDUINO_ARCH_ESP32)
    MySerial1.begin(9600, SERIAL_8N1, MY_RX1, MY_TX1);
  #else
    MySerial1.begin(9600);
  #endif
#endif
#ifdef MySerial2
  #if defined(ARDUINO_ARCH_ESP32)
    MySerial2.begin(9600, SERIAL_8N1, MY_RX2, MY_TX2);
  #else
    MySerial2.begin(9600);
  #endif
#endif
#ifdef MySerial3
  MySerial3.begin(9600);
#endif
}

void loop()
{
  static unsigned long count = 0;
#ifdef MySerial
  MySerial.print("Serial: ");
  MySerial.println(count);
#endif
#ifdef MySerialUSB
  MySerialUSB.print("SerialUSB: ");
  MySerialUSB.println(count);
#endif
#ifdef MySerial0
  MySerial0.print("Serial0: ");
  MySerial0.println(count);
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
  delay(1000);
}
