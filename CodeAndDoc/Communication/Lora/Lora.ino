/*
  RFM9xW LoRa modules

  - LoRa is like Walkie-Talkies, devices configured with the same radio 
    parameters and in range can see the packets you are sending.
  
  - RFM95W module (with RF96/SX1276 chip) available for 868 or 915 MHz.
    RFM96W module (with RF98/SX1278 chip) available for 433 or 470 MHz.
    RFM98W module (with RF98/SX1278 chip) available for 169 or 433 or 470 MHz.
  
  - VIN/3.3V   connect to a 3.3V source that can provide at least 120mA.
    MISO       connect to Arduino's MISO.
    MOSI       connect to Arduino's MOSI.
    SCK        connect to Arduino's SCK.
    NSS/CS     connect to pin defined by csPin.
    NRESET/RST connect to pin defined by resetPin.
    DIO0/G0    connect to pin defined by irqPin, it must be an interrupt
               capable pin which is only needed when registering the 
               receive callback with onReceive().
*/

#include "LoRa.h"         // https://github.com/sandeepmistry/arduino-LoRa
#include "CRC8.h"         // https://github.com/RobTillaart/CRC

// Pins connected to the LoRa module
const byte csPin = 8;     // defaults to 10
const byte resetPin = 4;  // defaults to 9
const byte irqPin = 7;    // defaults to 2

// Choose your network ID
const byte netID = 0x45;

// Global vars
CRC8 crc;
String msg;

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.println("LoRa Test: type in upper window and press ENTER");

  // If using pins other than the defaults set them with setPins()
  LoRa.setPins(csPin, resetPin, irqPin);
  
  /*
    Common frequencies:
    433E6 for Asia
    868E6 for Europe
    915E6 for North America and Australia
  */
  if (!LoRa.begin(868E6))
  {
    Serial.println("Starting LoRa failed!");
    while (true);
  }
}

void loop()
{
  // Get data from serial monitor's input field and send it
  if (Serial.available())
  {
    msg = Serial.readStringUntil('\n'); // function removes '\n' from serial buffer and does not return a '\n'
    msg.trim();                         // remove CR if terminal is sending one

    // Discard messages which are too long
    if (msg.length() < 230)
    {
      byte msgLength = (byte)msg.length();
      
      // Calc. CRC
      crc.restart();
      crc.add(netID);
      crc.add(msgLength);
      for (byte i = 0; i < msgLength; i++)
        crc.add(msg[i]);
      byte crcCalc = crc.calc();

      // Print info
      Serial.print("Sending: ");
      Serial.println(msg);
      
      // Send packet
      LoRa.beginPacket();
      LoRa.write(crcCalc);
      LoRa.write(netID);
      LoRa.write(msgLength);
      LoRa.print(msg);
      if (!LoRa.endPacket())
        Serial.println("Failed to send LoRa packet!");
    }
    else
      Serial.println("Message too long!");
  }

  // LoRa Packet received?
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // Read packet
    byte crcRx = LoRa.read();
    byte netIDRx = LoRa.read();
    byte msgLengthRx = LoRa.read();
    msg = "";
    while (LoRa.available())
      msg += (char)LoRa.read();

    // Checks
    if (netID != netIDRx)
      Serial.println("NOTE: other network ID");
    else if (msgLengthRx != msg.length())
      Serial.println("ERROR: incorrect message length!");
    else
    {
      // Calc. CRC
      crc.restart();
      crc.add(netIDRx);
      crc.add(msgLengthRx);
      for (byte i = 0; i < msgLengthRx; i++)
        crc.add(msg[i]);
      byte crcCalc = crc.calc();

      // Check CRC
      if (crcCalc != crcRx)
        Serial.println("ERROR: incorrect check-sum!");
      else
      {
        Serial.print("Received: ");
        Serial.print(msg);
        Serial.print(" (RSSI=");
        Serial.print(LoRa.packetRssi());
        Serial.print(", SNR=");
        Serial.print(LoRa.packetSnr());
        Serial.println(")");
      }
    }
  }
}
