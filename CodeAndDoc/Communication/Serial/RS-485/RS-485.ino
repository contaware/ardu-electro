/*
  MAX485 half-duplex RS-485 module

  - In this sketch we carry RS-232 data bits over RS-485. Connect two modules with
    their A&B lines, each module is controlled by an Arduino.
  
  - VCC 5V.
  
  - RS-485 uses a two-wire differential cabling such as a twisted pair. The cable 
    can extend up to 1200m and a maximum of 32 devices can connect to the same bus.
    Even though there can be 32 drivers on a line, only one driver can be active 
    at a time. The devices must agree on when they can use the bus to transmit 
    data (there are various protocols for that).
    If the Arduinos are distant, then the GND pins must be connected with cable 
    or through the ground of the electrical installation.
    The data-rate can be up to 2.5 Mbps, at 1200m it is 110 kbps.
  
  - Given the two differential lines A and B: when A - B >  0.2V it's a logic 1
                                              when A - B < -0.2V it's a logic 0
    The receiver input has a fail-safe feature that guarantees a logic 1 output
    when A and B are open circuit.
  
  - The common-mode voltage must be within -7V and +12V. The common-mode voltage 
    is the difference between signal A or B and its GND.

  - The two farthest ends of the bus need 120Ω terminations (module already has it).
    Note: devices in between would not require termination resistors.
  
  - A&B RS-485 signal lines
    DI  Driver input
    RO  Receiver output
    DE  Driver enable and RE Receiver enable (active LOW) jumpered together
*/

#include <SoftwareSerial.h>

// RX and TX pins
#define RX_RO_PIN         10
#define TX_DI_PIN         11
SoftwareSerial RS485Serial(RX_RO_PIN, TX_DI_PIN);

// Direction pin (necessary because we work in half-duplex mode)
#define DIR_CTRL_PIN      3
#define DIR_CTRL_TX       HIGH
#define DIR_CTRL_RX       LOW
// Note: we can get a full-duplex communication by employing two modules:
//       one is always sending and the other one is always receiving.

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.println("Use Serial Monitor, type in upper window and press ENTER");

  // Init module in receiver mode
  pinMode(DIR_CTRL_PIN, OUTPUT);
  digitalWrite(DIR_CTRL_PIN, DIR_CTRL_RX);

  // Start the software serial port
  RS485Serial.begin(9600);
}

void loop()
{
  // Get data from serial monitor's input field
  if (Serial.available())
  {
    int byteReceived = Serial.read();         // read received byte
    digitalWrite(DIR_CTRL_PIN, DIR_CTRL_TX);  // enable RS-485 transmit   
    RS485Serial.write(byteReceived);          // send byte to remote Arduino
    digitalWrite(DIR_CTRL_PIN, DIR_CTRL_RX);  // disable RS-485 transmit       
  }

  // Look for data from the other Arduino
  if (RS485Serial.available())
  {
    int byteReceived = RS485Serial.read();    // read received byte
    Serial.write(byteReceived);               // show on serial monitor
  }
}
