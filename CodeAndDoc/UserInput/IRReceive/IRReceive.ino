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

#define IR_RECEIVE_PIN  11
unsigned long last_key_value = 0;

void TranslateIR(unsigned long value)
{
  switch(value)
  {
    case 0xB946FF00: Serial.println("UP"); break;
    case 0xBB44FF00: Serial.println("LEFT"); break;
    case 0xBF40FF00: Serial.println("OK"); break;
    case 0xBC43FF00: Serial.println("RIGHT"); break;
    case 0xEA15FF00: Serial.println("DOWN"); break;
    case 0xE916FF00: Serial.println("1"); break;
    case 0xE619FF00: Serial.println("2"); break;
    case 0xF20DFF00: Serial.println("3"); break;
    case 0xF30CFF00: Serial.println("4"); break;
    case 0xE718FF00: Serial.println("5"); break;
    case 0xA15EFF00: Serial.println("6"); break;
    case 0xF708FF00: Serial.println("7"); break;
    case 0xE31CFF00: Serial.println("8"); break;
    case 0xA55AFF00: Serial.println("9"); break;
    case 0xBD42FF00: Serial.println("*"); break;
    case 0xAD52FF00: Serial.println("0"); break;
    case 0xB54AFF00: Serial.println("#"); break;
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
  if (IrReceiver.decode()) // have we received an IR signal?
  {
    // Print info
    //IrReceiver.printIRResultShort(&Serial);

    // Get value
    if (IrReceiver.decodedIRData.protocol == NEC)
    {
      unsigned long current_key_value = IrReceiver.decodedIRData.decodedRawData;
      if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)
        current_key_value = last_key_value;

      if (current_key_value != 0)
      {
        TranslateIR(current_key_value);
        last_key_value = current_key_value;
      }
      else
        last_key_value = 0;
    }
    else
      last_key_value = 0;

    // Enable receiving of the next value
    IrReceiver.resume();
  }
}
