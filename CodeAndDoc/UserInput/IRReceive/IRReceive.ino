/*
  Specify which protocol(s) should be used for decoding.
  If no protocol is defined, all protocols are active.
*/
//#define DECODE_DENON        // Includes Sharp
//#define DECODE_JVC
//#define DECODE_KASEIKYO
//#define DECODE_PANASONIC    // the same as DECODE_KASEIKYO
//#define DECODE_LG
#define DECODE_NEC          // Includes Apple and Onkyo
//#define DECODE_SAMSUNG
//#define DECODE_SONY
//#define DECODE_RC5
//#define DECODE_RC6

//#define DECODE_BOSEWAVE
//#define DECODE_LEGO_PF
//#define DECODE_MAGIQUEST
//#define DECODE_WHYNTER

//#define DECODE_DISTANCE     // universal decoder for pulse width or pulse distance protocols
//#define DECODE_HASH         // special decoder for all protocols

//#define DEBUG               // Activate this for lots of lovely debug output from the decoders.
//#define INFO                // To see valuable informations from universal decoder for pulse width or pulse distance protocols
#include <IRremote.hpp>

#define IR_RECEIVE_PIN  12

long lastKeyValue = -1; // -1 means invalid or not set

void TranslateIR(long value)
{
  switch(value)
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
      Serial.print("Other button = 0x");
      Serial.println(value, HEX);
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop()
{
  // Have we received an IR signal?
  if (IrReceiver.decode())
  {
    // Print info
    //IrReceiver.printIRResultShort(&Serial);

    // Get value
    if (IrReceiver.decodedIRData.protocol == NEC)
    {
      // Get key value
      long currentKeyValue = IrReceiver.decodedIRData.command; // command is a uint16_t
      if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)
        currentKeyValue = lastKeyValue;

      // Valid?
      if (currentKeyValue >= 0)
      {
        TranslateIR(currentKeyValue);
        lastKeyValue = currentKeyValue;
      }
      else
        lastKeyValue = -1;
    }
    else
      lastKeyValue = -1;

    // Enable receiving of the next value
    IrReceiver.resume();
  }
}
