/*
  IR Sender

  - VDD supply is 3.3V - 5V.

  - To mitigate interference from the sunlight or from other light 
    sources, the IR signals are modulated, typically at 38kHz.

  - To be consistent with IR receivers, let's define a HIGH when 
    no IR signal is sent, and a LOW when a IR carrier frequency 
    is sent. The NEC protocol is the most common:
    * Logical 0 is transmitted with a 562.5µs long LOW pulse, 
      followed by a 562.5µs long HIGH pulse.
    * Logical 1 is transmitted with a 562.5µs long LOW pulse, 
      followed by a 1687.5µs (3x 562.5µs) long HIGH pulse.
*/

// Disables static receiver code like receive timer ISR handler and static 
// IRReceiver and irparams data. Saves 450 bytes program memory and 269 bytes
// RAM if receiving functions are not required.
#if !defined(ARDUINO_ESP32C3_DEV) // this is due to a bug in RISC-V compiler, which requires unused function sections :-(
#define DISABLE_CODE_FOR_RECEIVER
#endif
#define NO_LED_FEEDBACK_CODE      // saves 566 bytes program memory
#define IR_SEND_PIN           12

#include <IRremote.hpp>           // IRremote by shirriff, z3t0, ArminJo, https://github.com/Arduino-IRremote/Arduino-IRremote

uint8_t CharToCommand(char c)
{
  switch (toupper(c))
  {
    case 'U': return 0x46; // UP
    case 'L': return 0x44; // LEFT
    case 'O': return 0x40; // OK
    case 'R': return 0x43; // RIGHT
    case 'D': return 0x15; // DOWN
    case '1': return 0x16; // 1
    case '2': return 0x19; // 2
    case '3': return 0xD;  // 3
    case '4': return 0xC;  // 4
    case '5': return 0x18; // 5
    case '6': return 0x5E; // 6
    case '7': return 0x8;  // 7
    case '8': return 0x1C; // 8
    case '9': return 0x5A; // 9
    case '*': return 0x42; // *
    case '0': return 0x52; // 0
    case '#': return 0x4A; // #
    default: return 0;
  }
}

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.print("IRremote version " VERSION_IRREMOTE);
  Serial.println(": type in upper window and press ENTER");

  // Init IR sender with IR_SEND_PIN
  IrSender.begin();
}

void loop()
{
  // Get data from serial monitor's input field
  if (Serial.available())
  {
    String msg;
    msg = Serial.readStringUntil('\n'); // function removes '\n' from serial buffer and does not return a '\n'
    msg.trim();                         // remove CR if terminal is sending one
    while (msg.length() > 0)
    {
      uint16_t Address = 0x00; // 8-bit or 16-bit address
      uint8_t Command = CharToCommand(msg[0]);
      uint8_t Repeats = 0;
      if (Command > 0)
      {
        Serial.print("Addr 0x"); Serial.print(Address, HEX);
        Serial.print(": 0x"); Serial.println(Command, HEX);
        IrSender.sendNEC(Address, Command, Repeats);
        delay(80);      // needs a delay, otherwise the rx sees it as one long signal
      }
      msg.remove(0, 1); // remove first char
      msg.trim();       // trim whitespaces
    }
  }
}
