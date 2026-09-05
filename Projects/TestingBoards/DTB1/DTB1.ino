/*
  Digital Testing Board to verify digital chips with Arduino

  - The chip to be verified is called DUT (device under test).

  - This project has 10 outputs and 8 inputs.
    * The last output is connected to the built-in LED and 
      can also be connected to the DUT without any problems.
    * This tester is simple: it does not use any interrupts, 
      so only slowly changing signals are detected at the 8 inputs.

  - I implemented the tester making an Arduino UNO shield with the 
    8 inputs buffered through a 74AHCT244 to support DUTs with TTL 
    outputs, such as the 74LSxxx chip family.
*/
// Tester outputs map to the following Arduino pins
#define TESTER_OUT0_PIN      2
#define TESTER_OUT1_PIN      3
#define TESTER_OUT2_PIN      4
#define TESTER_OUT3_PIN      5
#define TESTER_OUT4_PIN      6
#define TESTER_OUT5_PIN      7
#define TESTER_OUT6_PIN      8
#define TESTER_OUT7_PIN      9
#define TESTER_OUT8_PIN      10
#define TESTER_OUT9_PIN      LED_BUILTIN
#define TESTER_OUT_LAST      9

// Tester inputs map to the following Arduino pins
#define TESTER_IN0_PIN       11
#define TESTER_IN1_PIN       12
#define TESTER_IN2_PIN       A0
#define TESTER_IN3_PIN       A1
#define TESTER_IN4_PIN       A2
#define TESTER_IN5_PIN       A3
#define TESTER_IN6_PIN       A4
#define TESTER_IN7_PIN       A5

// Pulse length in us
const unsigned int PULSE_LENGTH_US = 50;

// To be compatible with all platforms keep track of the tester outputs
// bit0 = OUT0
// ...
// bit7 = OUT7
//
// bit8 = OUT8
// bit9 = OUT9 (LED)
uint16_t g_out = 0;

// Tester inputs
bool g_pollInputs = true;
uint8_t g_in = 0;
uint8_t g_inMask = 0xFF;

void printCmds()
{
  Serial.println("Type in upper window and press ENTER:");
  Serial.println("?          : Show this help");
  Serial.println("H0..H9     : Set given output HIGH");
  Serial.println("L0..L9     : Set given output LOW");
  Serial.print("P0..P9     : Pulse given output");
  Serial.print(" (");
  Serial.print(PULSE_LENGTH_US);
  Serial.println("us)");
  Serial.println("value      : Set outputs 7..0 to BIN or HEX starting with 0x");
  Serial.println("ENTER      : Show outputs and inputs");
  Serial.println("I          : Toggle input change display (default ON)");
  Serial.println("Mvalue     : Set input mask 7..0 to BIN or HEX starting with 0x");
}

int TesterOutToPin(int arduOut)
{
  switch (arduOut)
  {
    case 0: return TESTER_OUT0_PIN;
    case 1: return TESTER_OUT1_PIN;
    case 2: return TESTER_OUT2_PIN;
    case 3: return TESTER_OUT3_PIN;
    case 4: return TESTER_OUT4_PIN;
    case 5: return TESTER_OUT5_PIN;
    case 6: return TESTER_OUT6_PIN;
    case 7: return TESTER_OUT7_PIN;
    case 8: return TESTER_OUT8_PIN;
    default: return TESTER_OUT9_PIN;
  }
}

void writeOutput(int outNum, int outValue)
{
  outNum = constrain(outNum, 0, TESTER_OUT_LAST);
  int outPin = TesterOutToPin(outNum);

  if (outValue)
  {
    // Change output
    digitalWrite(outPin, HIGH);

    // Print and update output variable
    printOutputChange(outNum, bitRead(g_out, outNum), true);
    bitWrite(g_out, outNum, 1);
  }
  else
  {
    // Change output
    digitalWrite(outPin, LOW);

    // Print and update output variable
    printOutputChange(outNum, bitRead(g_out, outNum), false);
    bitWrite(g_out, outNum, 0);
  }
}

void pulseOutput(int outNum)
{
  outNum = constrain(outNum, 0, TESTER_OUT_LAST);
  int outPin = TesterOutToPin(outNum);

  // Read current output value to decide the pulse type
  if (bitRead(g_out, outNum))
  {
    // Low pulse
    digitalWrite(outPin, LOW);
    delayMicroseconds(PULSE_LENGTH_US);
    digitalWrite(outPin, HIGH);
    delayMicroseconds(PULSE_LENGTH_US);

    // Print
    printPulse(outNum, false);
  }
  else
  {
    // High pulse
    digitalWrite(outPin, HIGH);
    delayMicroseconds(PULSE_LENGTH_US);
    digitalWrite(outPin, LOW);
    delayMicroseconds(PULSE_LENGTH_US);

    // Print
    printPulse(outNum, true);
  }
}

void writeOutputs8(uint8_t outValue)
{
  digitalWrite(TESTER_OUT0_PIN, bitRead(outValue, 0) ? HIGH : LOW);
  digitalWrite(TESTER_OUT1_PIN, bitRead(outValue, 1) ? HIGH : LOW);
  digitalWrite(TESTER_OUT2_PIN, bitRead(outValue, 2) ? HIGH : LOW);
  digitalWrite(TESTER_OUT3_PIN, bitRead(outValue, 3) ? HIGH : LOW);
  digitalWrite(TESTER_OUT4_PIN, bitRead(outValue, 4) ? HIGH : LOW);
  digitalWrite(TESTER_OUT5_PIN, bitRead(outValue, 5) ? HIGH : LOW);
  digitalWrite(TESTER_OUT6_PIN, bitRead(outValue, 6) ? HIGH : LOW);
  digitalWrite(TESTER_OUT7_PIN, bitRead(outValue, 7) ? HIGH : LOW);
  g_out = (g_out & ~255U) | (uint16_t)outValue;
}

void printPulse(int outNum, bool highPulse)
{
  Serial.print("OUT[");
  Serial.print(outNum);
  Serial.print("]     : ");

  if (highPulse)
    Serial.print("__--__");
  else
    Serial.print("--__--");

  Serial.print(" (");
  Serial.print(PULSE_LENGTH_US);
  Serial.println("us)");
}

void printOutputChange(int outNum, bool highInit, bool highNow)
{
  Serial.print("OUT[");
  Serial.print(outNum);
  Serial.print("]     : ");

  if (highInit)
    Serial.print("--");
  else
    Serial.print("__");

  if (highNow)
    Serial.print("--");
  else
    Serial.print("__");

  Serial.print(" ");
  Serial.println(highNow ? "1" : "0");
}

void printOutputs()
{
  Serial.print("OUT[7..0]  : ");
  for (int i = 7 ; i >= 0 ; i--)
  {
    if (i == 3) Serial.print(" ");
    Serial.print(bitRead(g_out, i));
  }
  uint8_t lowB = lowByte(g_out);
  Serial.print(" (0x");
  if (lowB < 0x10) Serial.print('0');
  Serial.print(lowB, HEX);
  Serial.print(") , ");

  Serial.print("OUT[9]: ");
  Serial.print(bitRead(g_out, 9));
  Serial.print(" , OUT[8]: ");
  Serial.println(bitRead(g_out, 8));
}

bool readInputs()
{
  // Read the Inputs, treat the disabled Inputs as 0
  uint8_t in = 0;
  bitWrite(in, 7, bitRead(g_inMask, 7) ? digitalRead(TESTER_IN7_PIN) : 0);
  bitWrite(in, 6, bitRead(g_inMask, 6) ? digitalRead(TESTER_IN6_PIN) : 0);
  bitWrite(in, 5, bitRead(g_inMask, 5) ? digitalRead(TESTER_IN5_PIN) : 0);
  bitWrite(in, 4, bitRead(g_inMask, 4) ? digitalRead(TESTER_IN4_PIN) : 0);
  bitWrite(in, 3, bitRead(g_inMask, 3) ? digitalRead(TESTER_IN3_PIN) : 0);
  bitWrite(in, 2, bitRead(g_inMask, 2) ? digitalRead(TESTER_IN2_PIN) : 0);
  bitWrite(in, 1, bitRead(g_inMask, 1) ? digitalRead(TESTER_IN1_PIN) : 0);
  bitWrite(in, 0, bitRead(g_inMask, 0) ? digitalRead(TESTER_IN0_PIN) : 0);

  // Check whether at least one of the Inputs changed
  bool changed = (in != g_in); 

  // Update global variable
  g_in = in;

  return changed;
}

void printInputs()
{
  Serial.print("IN[7..0]   : ");
  for (int i = 7 ; i >= 0 ; i--)
  {
    if (i == 3) Serial.print(" ");
    Serial.print(bitRead(g_in, i));
  }
  Serial.print(" (0x");
  if (g_in < 0x10) Serial.print('0');
  Serial.print(g_in, HEX);
  Serial.print(") , ");

  Serial.print("MASK[7..0]: ");
  for (int i = 7 ; i >= 0 ; i--)
  {
    if (i == 3) Serial.print(" ");
    Serial.print(bitRead(g_inMask, i));
  }
  Serial.println();
}

uint8_t to8(const String& msg)
{
  // Convert given String to a uint8_t
  // Note: strtol() returns 0 if conversion fails.
  if (msg.length() >= 3 && (msg[0] == '0' && tolower(msg[1]) == 'x'))
    return (uint8_t)strtol(msg.c_str(), nullptr, 16); // HEX
  else
    return (uint8_t)strtol(msg.c_str(), nullptr, 2);  // BIN
}

void doSerialRead()
{
  String msg;
  msg = Serial.readStringUntil('\n'); // function removes '\n' from serial buffer and does not return a '\n'
  msg.trim();                         // remove CR if terminal is sending one
  if (msg.length() == 0)              // if just pressing ENTER
  {
    printOutputs();
    readInputs();
    printInputs();
    return;
  }

  switch (toupper(msg[0]))
  {
    // ATTENTION: do not use 'A', 'B', 'C', 'D', 'E', 'F'
    //            as commands because they are for hex values!
    case '?':
      printCmds();
      break;

    case 'H':
      if (msg.length() >= 2 && isdigit(msg[1]))
      {
        msg.remove(0, 1);             // remove 'H' char
        int outNum = msg.toInt();     // returns 0 if conversion fails
        writeOutput(outNum, 1);
      }
      else
        Serial.println("ERROR      : After 'H' type an output number");
      break;

    case 'L':
      if (msg.length() >= 2 && isdigit(msg[1]))
      {
        msg.remove(0, 1);             // remove 'L' char
        int outNum = msg.toInt();     // returns 0 if conversion fails
        writeOutput(outNum, 0);
      }
      else
        Serial.println("ERROR      : After 'L' type an output number");
      break;

    case 'P':
      if (msg.length() >= 2 && isdigit(msg[1]))
      {
        msg.remove(0, 1);             // remove 'P' char
        int outNum = msg.toInt();     // returns 0 if conversion fails
        pulseOutput(outNum);
      }
      else
        Serial.println("ERROR      : After 'P' type an output number");
      break;

    case 'I':
      g_pollInputs = !g_pollInputs;
      break;

    case 'M':
      if (msg.length() >= 2 && (msg[1] == '0' || msg[1] == '1'))
      {
        msg.remove(0, 1);             // remove 'M' char
        g_inMask = to8(msg);          // returns 0 if conversion fails
        readInputs();
        printInputs();
      }
      else
        Serial.println("ERROR      : After 'M' type a BIN or a HEX starting with 0x");
      break;
    
    default:
      if (msg.length() >= 1 && (msg[0] == '0' || msg[0] == '1'))
      {
        writeOutputs8(to8(msg));      // to8() returns 0 if conversion fails
        printOutputs();
      }
      else
        Serial.println("ERROR      : Type a BIN or a HEX starting with 0x");
      break;
  }
}

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready

  // Init Arduino input pins
  // Note: usually all pins default to INPUT,
  //       just to make sure it works on all platforms.
  pinMode(TESTER_IN0_PIN, INPUT);
  pinMode(TESTER_IN1_PIN, INPUT);
  pinMode(TESTER_IN2_PIN, INPUT);
  pinMode(TESTER_IN3_PIN, INPUT);
  pinMode(TESTER_IN4_PIN, INPUT);
  pinMode(TESTER_IN5_PIN, INPUT);
  pinMode(TESTER_IN6_PIN, INPUT);
  pinMode(TESTER_IN7_PIN, INPUT);

  // Init Arduino output pins
  pinMode(TESTER_OUT0_PIN, OUTPUT);
  pinMode(TESTER_OUT1_PIN, OUTPUT);
  pinMode(TESTER_OUT2_PIN, OUTPUT);
  pinMode(TESTER_OUT3_PIN, OUTPUT);
  pinMode(TESTER_OUT4_PIN, OUTPUT);
  pinMode(TESTER_OUT5_PIN, OUTPUT);
  pinMode(TESTER_OUT6_PIN, OUTPUT);
  pinMode(TESTER_OUT7_PIN, OUTPUT);
  pinMode(TESTER_OUT8_PIN, OUTPUT);
  pinMode(TESTER_OUT9_PIN, OUTPUT);
  digitalWrite(TESTER_OUT0_PIN, LOW);
  digitalWrite(TESTER_OUT1_PIN, LOW);
  digitalWrite(TESTER_OUT2_PIN, LOW);
  digitalWrite(TESTER_OUT3_PIN, LOW);
  digitalWrite(TESTER_OUT4_PIN, LOW);
  digitalWrite(TESTER_OUT5_PIN, LOW);
  digitalWrite(TESTER_OUT6_PIN, LOW);
  digitalWrite(TESTER_OUT7_PIN, LOW);
  digitalWrite(TESTER_OUT8_PIN, LOW);
  digitalWrite(TESTER_OUT9_PIN, LOW);

  // Print Help, Outputs and Inputs
  printCmds();
  printOutputs();
  readInputs(); // do a first read to init g_in
  printInputs();
}

void loop()
{
  if (Serial.available())
    doSerialRead();

  if (g_pollInputs && readInputs())
    printInputs();
}
