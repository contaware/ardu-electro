/*
  IR Receiver

  - VDD supply is 3V - 5V.

  - To mitigate interference from the sunlight or from other light 
    sources, the IR signals are modulated, typically at 38kHz.

  - The IR Receiver demodulates the IR signal and outputs HIGH 
    when there is no IR signal and LOW when a IR carrier frequency 
    is detected. The NEC protocol is the most common:
    * Logical 0 is transmitted with a 562.5µs long LOW pulse, 
      followed by a 562.5µs long HIGH pulse.
    * Logical 1 is transmitted with a 562.5µs long LOW pulse, 
      followed by a 1687.5µs (3x 562.5µs) long HIGH pulse.

  - Module pinout (KY-022): 
    G=GND, R=VDD, Y=Signal
  
  - 1838/TSOP4838 receiver pinout (front view): 
    1=Signal, 2=GND, 3=VDD
*/

// Specify which protocol(s) should be used for decoding
// - If no protocol is defined, all protocols are active.
// - This must be done before the #include <IRremote.hpp>
// - For the possible defines check this example:
//   https://github.com/Arduino-IRremote/Arduino-IRremote/blob/master/examples/SimpleReceiver/SimpleReceiver.ino
#define DECODE_NEC
#include <IRremote.hpp> // IRremote by shirriff, z3t0, ArminJo, https://github.com/Arduino-IRremote/Arduino-IRremote

#define IR_RECEIVE_PIN  12

long lastKeyValue = -1; // -1 means invalid or not set

void TranslateIR(uint16_t value, bool repeat)
{
  if (repeat)
    Serial.print("Repeat of ");

  switch (value)
  {
    case 0x46:  Serial.println("UP"); break;
    case 0x44:  Serial.println("LEFT"); break;
    case 0x40:  Serial.println("OK"); break;
    case 0x43:  Serial.println("RIGHT"); break;
    case 0x15:  Serial.println("DOWN"); break;
    case 0x16:  Serial.println("1"); break;
    case 0x19:  Serial.println("2"); break;
    case 0xD:   Serial.println("3"); break;
    case 0xC:   Serial.println("4"); break;
    case 0x18:  Serial.println("5"); break;
    case 0x5E:  Serial.println("6"); break;
    case 0x8:   Serial.println("7"); break;
    case 0x1C:  Serial.println("8"); break;
    case 0x5A:  Serial.println("9"); break;
    case 0x42:  Serial.println("*"); break;
    case 0x52:  Serial.println("0"); break;
    case 0x4A:  Serial.println("#"); break;
    default: 
      Serial.print("0x");
      Serial.println(value, HEX);
  }
}

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.println("IRremote version " VERSION_IRREMOTE);

  // Init IR receiver
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop()
{
  // Check if received data is available and if yes, try to decode it.
  // The decoded result is placed in the IrReceiver.decodedIRData structure.
  if (IrReceiver.decode())
  {
    // Enable receiving of the next IR frame
    IrReceiver.resume();

    if (IrReceiver.decodedIRData.protocol == NEC)
    {
      if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)
      {
        if (lastKeyValue >= 0)
          TranslateIR((uint16_t)lastKeyValue, true);
        else
          Serial.println("Repeat of Not NEC");
      }
      else
      {
        TranslateIR(IrReceiver.decodedIRData.command, false);
        lastKeyValue = IrReceiver.decodedIRData.command;
      }
    }
    else
    {
      lastKeyValue = -1;
      Serial.println("Not NEC");
    }
  }
}
