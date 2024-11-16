/*
  MAX485 and MAX3485 half-duplex RS-485 modules

  - In this sketch we carry RS-232 data bits over RS-485. Connect two 
    modules with their A&B lines, each module is controlled by an 
    Arduino having this sketch.
  
  - VCC of MAX485 is 5V and of MAX3485 is 3.3V.
  
  - RS-485 uses a two-wire differential cabling such as a twisted pair. 
    The cable can extend up to 1200m and a maximum of 32 devices can 
    connect to the same bus. Even though there can be 32 drivers on a 
    line, only one driver can be active at a time. The devices must 
    agree on when they can use the bus to transmit data (there are 
    various protocols for that). If the Arduinos are distant, then the 
    GND pins must be connected with cable or through the ground of the 
    electrical installation. The data-rate can be up to 2.5 Mbps, at 
    1200m it is 110 kbps.
  
  - Given the two differential lines A and B: 
    * when A - B >  0.2V it's a logic 1
    * when A - B < -0.2V it's a logic 0
    The receiver input has a fail-safe feature that guarantees a logic 1 
    output when A and B are open circuit.
  
  - The common-mode voltage must be within -7V and +12V. The common-mode 
    voltage is the average of the voltages at A and B with respect to GND.
    
  - The two farthest ends of the bus need 120Ω terminations 
    (module already has it).
    Note: devices in between would not require termination resistors.
  
  - A   connected to A of the other module.
    B   connected to B of the other module.
    DI  Driver input connected to Arduino's TX pin.
    RO  Receiver output connected to Arduino's RX pin.
    DE  Driver enable and RE Receiver enable (active LOW) jumpered 
        together and connected to Arduino's DIR_CTRL_PIN. 
*/

// For boards that do not have a dedicated serial port 
// (like Uno and Nano) set to 1, otherwise to 0.
// Note: ESP8266 only supports TX on Serial1, so you may want to choose 
//       the software serial for that architecture.
#define USE_SOFTWARE_SERIAL   0

#if USE_SOFTWARE_SERIAL == 1
#include <SoftwareSerial.h>
#define RX_RO_PIN             10
#define TX_DI_PIN             11
SoftwareSerial RS485Serial(RX_RO_PIN, TX_DI_PIN);
#else
// For ESP32 define the pins:
#define RX_RO_PIN             4
#define TX_DI_PIN             5
// For most boards:           0(RX),  1(TX)
// Due and Mega:              19(RX), 18(TX)
// MKR boards:                13(RX), 14(TX)
// ESP8266:                   no RX,  GPIO2(TX)
#define RS485Serial           Serial1
#endif

// Direction pin (necessary because we work in half-duplex mode)
#define DIR_CTRL_PIN          3
#define DIR_CTRL_TX           HIGH
#define DIR_CTRL_RX           LOW
// Note: we can get a full-duplex communication by employing two modules:
//       one is always sending and the other one is always receiving.

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready

  // Init module in receiver mode
  pinMode(DIR_CTRL_PIN, OUTPUT);
  digitalWrite(DIR_CTRL_PIN, DIR_CTRL_RX);
  
#if USE_SOFTWARE_SERIAL == 1
  Serial.print("Using Software Serial on RX pin="); Serial.print(RX_RO_PIN);
  Serial.print(" and TX pin=");                     Serial.println(TX_DI_PIN);
  RS485Serial.begin(9600);
#else
  Serial.println("Using Hardware Serial");
  #if defined(ARDUINO_ARCH_ESP32)
    RS485Serial.begin(9600, SERIAL_8N1, RX_RO_PIN, TX_DI_PIN);
  #else
    RS485Serial.begin(9600);
  #endif
#endif
  Serial.println("RS-485 Test: type in upper window and press ENTER");
}

void loop()
{
  // Get data from serial monitor's input field
  if (Serial.available() > 0)
  {
    int byteReceived = Serial.read();         // read received byte
    digitalWrite(DIR_CTRL_PIN, DIR_CTRL_TX);  // enable RS-485 transmit
    RS485Serial.write(byteReceived);          // if there is enough empty space in the tx buf, write() will return
                                              // before any chars are transmitted, but if the tx buf is full, then
                                              // write() will block until there is enough space.
    RS485Serial.flush();                      // waits until all data has been clocked out
    //delay(2);                               // flush() can be buggy, uncomment if having problems
    digitalWrite(DIR_CTRL_PIN, DIR_CTRL_RX);  // disable RS-485 transmit
  }

  // Look for data from the other Arduino
  if (RS485Serial.available() > 0)
  {
    int byteReceived = RS485Serial.read();    // read received byte
    Serial.write(byteReceived);               // show on serial monitor
  }
}
