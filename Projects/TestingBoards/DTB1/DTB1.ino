/*
  Digital Testing Board to verify digital chips with Arduino

  - The chip to be verified is called DUT (device under test).

  - This project has 10 outputs and 8 inputs.
    * The last output is connected to the built-in LED and 
      can also be connected to the DUT without any problems.
    * This tester is simple, it does not use any interrupts.

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

// Timing constants
// - SIG_SETTLE_US is used after each write to let the signal settle
// - a pulse length in microseconds is SIG_SETTLE_US + PULSE_HOLD_US
const unsigned long SIG_SETTLE_US = 10;
const unsigned long PULSE_HOLD_US = 40;

// To be compatible with all platforms keep track of the tester outputs
// bit0 = OUT0
// ...
// bit7 = OUT7
//
// bit8 = OUT8
// bit9 = OUT9 (LED)
uint16_t g_out = 0;

// Tester inputs
uint8_t g_in = 0;
uint8_t g_inMask = 0xFF;

void printCmds()
{
  Serial.println("Type space-separated commands in upper window and press ENTER:");
  Serial.println("?          : Show this help");
  Serial.println("ENTER      : Show outputs and inputs");
  Serial.println("H0..H9     : Set given output HIGH");
  Serial.println("L0..L9     : Set given output LOW");
  Serial.print("P0..P9     : Pulse given output");
  Serial.print(" (");
  Serial.print(SIG_SETTLE_US + PULSE_HOLD_US);
  Serial.println("us)");
  Serial.println("value      : Set outputs 9..0 to BIN or HEX starting with 0x");
  Serial.println("Mvalue     : Set input mask 7..0 to BIN or HEX starting with 0x");
  Serial.println("Wvalue     : Wait given milliseconds, 1s if no value");
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
  outValue = constrain(outValue, 0, 1);

  // Check whether the wanted value is already set
  if (outValue == bitRead(g_out, outNum))
  {
    Serial.print("OUT[");
    Serial.print(outNum);
    Serial.print("]     : Output already ");
    Serial.println(outValue);
    return;
  }

  // Read Inputs before the Change
  readInputs();

  // Change Output and let it settle
  digitalWrite(outPin, outValue ? HIGH : LOW);
  delayMicroseconds(SIG_SETTLE_US);

  // Update output variable and Print
  bitWrite(g_out, outNum, outValue);
  printOutputChange(outNum, outValue);

  // If Inputs changed, show the Inputs
  if (readInputs())
    printInputs();
}

void pulseOutput(int outNum)
{
  outNum = constrain(outNum, 0, TESTER_OUT_LAST);
  int outPin = TesterOutToPin(outNum);

  // Read current output value to decide the pulse direction
  bool highPulse = !bitRead(g_out, outNum);

  // Read Inputs before the Pulse
  readInputs();

  // Pulse Output and let it settle
  digitalWrite(outPin, highPulse ? HIGH : LOW);
  delayMicroseconds(SIG_SETTLE_US);

  // Leave Output unchanged for PULSE_HOLD_US and, in the meantime, read Inputs
  unsigned long startTime = micros();
  bool inputChanged = readInputs();
  unsigned long elapsedTime = micros() - startTime; 
  if (elapsedTime < PULSE_HOLD_US)
    delayMicroseconds(PULSE_HOLD_US - elapsedTime);

  // Return Output to its initial state and let it settle
  digitalWrite(outPin, highPulse ? LOW : HIGH);
  delayMicroseconds(SIG_SETTLE_US);

  // Print
  printPulse(outNum, highPulse);

  // If Inputs changed during the pulse, show the Inputs
  if (inputChanged)
    printInputs();

  // If Inputs changed after the pulse ended, show the Inputs
  if (readInputs())
    printInputs();
}

void writeOutputs(uint16_t outValue)
{
  const uint16_t maxValue = (1U << (TESTER_OUT_LAST + 1)) - 1U;
  outValue = constrain(outValue, 0, maxValue);

  // Read Inputs before the Change
  readInputs();

  // Change Outputs and let them settle
  digitalWrite(TESTER_OUT0_PIN, bitRead(outValue, 0) ? HIGH : LOW);
  digitalWrite(TESTER_OUT1_PIN, bitRead(outValue, 1) ? HIGH : LOW);
  digitalWrite(TESTER_OUT2_PIN, bitRead(outValue, 2) ? HIGH : LOW);
  digitalWrite(TESTER_OUT3_PIN, bitRead(outValue, 3) ? HIGH : LOW);
  digitalWrite(TESTER_OUT4_PIN, bitRead(outValue, 4) ? HIGH : LOW);
  digitalWrite(TESTER_OUT5_PIN, bitRead(outValue, 5) ? HIGH : LOW);
  digitalWrite(TESTER_OUT6_PIN, bitRead(outValue, 6) ? HIGH : LOW);
  digitalWrite(TESTER_OUT7_PIN, bitRead(outValue, 7) ? HIGH : LOW);
  digitalWrite(TESTER_OUT8_PIN, bitRead(outValue, 8) ? HIGH : LOW);
  digitalWrite(TESTER_OUT9_PIN, bitRead(outValue, 9) ? HIGH : LOW);
  delayMicroseconds(SIG_SETTLE_US);

  // Update output variable and Print
  g_out = outValue;
  printOutputs();

  // If Inputs changed, show the Inputs
  if (readInputs())
    printInputs();
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
  Serial.print(SIG_SETTLE_US + PULSE_HOLD_US);
  Serial.println("us)");
}

void printOutputChange(int outNum, int outValue)
{
  Serial.print("OUT[");
  Serial.print(outNum);
  Serial.print("]     : ");

  if (outValue)
    Serial.print("__--");
  else
    Serial.print("--__");

  Serial.print(" ");
  Serial.println(outValue);
}

void printOutputs()
{
  Serial.print("OUT[9..0]  : ");
  for (int i = 9 ; i >= 0 ; i--)
  {
    if (i == 7 || i == 3) Serial.print(" ");
    Serial.print(bitRead(g_out, i));
  }
  Serial.print(" (0x");
  if (g_out < 0x100) Serial.print('0');
  if (g_out < 0x10) Serial.print('0');
  Serial.print(g_out, HEX);
  Serial.println(")");
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
  long outValue;
  if (msg.length() >= 3 && (msg[0] == '0' && tolower(msg[1]) == 'x'))
    outValue = strtol(msg.c_str(), nullptr, 16); // HEX
  else
    outValue = strtol(msg.c_str(), nullptr, 2);  // BIN
  return (uint8_t)constrain(outValue, 0L, 255L);
}

uint16_t to16(const String& msg)
{
  // Convert given String to a uint16_t
  // Note: strtol() returns 0 if conversion fails.
  long outValue;
  if (msg.length() >= 3 && (msg[0] == '0' && tolower(msg[1]) == 'x'))
    outValue = strtol(msg.c_str(), nullptr, 16); // HEX
  else
    outValue = strtol(msg.c_str(), nullptr, 2);  // BIN
  return (uint16_t)constrain(outValue, 0L, 65535L);
}

bool parseCmd(String& cmd)
{
  switch (toupper(cmd[0]))
  {
    // ATTENTION: do not use 'A', 'B', 'C', 'D', 'E', 'F'
    //            as commands because they are for hex values!
    case 'H':
      if (cmd.length() >= 2 && isdigit(cmd[1]))
      {
        cmd.remove(0, 1);             // remove 'H' char
        int outNum = cmd.toInt();     // returns 0 if conversion fails
        writeOutput(outNum, 1);
        return true;
      }
      else
      {
        Serial.println("ERROR      : After 'H' type an output number");
        return false;
      }

    case 'L':
      if (cmd.length() >= 2 && isdigit(cmd[1]))
      {
        cmd.remove(0, 1);             // remove 'L' char
        int outNum = cmd.toInt();     // returns 0 if conversion fails
        writeOutput(outNum, 0);
        return true;
      }
      else
      {
        Serial.println("ERROR      : After 'L' type an output number");
        return false;
      }

    case 'P':
      if (cmd.length() >= 2 && isdigit(cmd[1]))
      {
        cmd.remove(0, 1);             // remove 'P' char
        int outNum = cmd.toInt();     // returns 0 if conversion fails
        pulseOutput(outNum);
        return true;
      }
      else
      {
        Serial.println("ERROR      : After 'P' type an output number");
        return false;
      }

    case 'M':
      if (cmd.length() >= 2 && (cmd[1] == '0' || cmd[1] == '1'))
      {
        cmd.remove(0, 1);             // remove 'M' char
        g_inMask = to8(cmd);          // returns 0 if conversion fails
        readInputs();                 // read inputs and
        printInputs();                // print them (new mask is also shown)
        return true;
      }
      else
      {
        Serial.println("ERROR      : After 'M' type a BIN or a HEX starting with 0x");
        return false;
      }

    case 'W':
      if (cmd.length() == 1)
      {
        delay(1000);
        return true;
      }
      else
      {
        cmd.remove(0, 1);             // remove 'W' char
        // Convert given String to an unsigned long
        // Note: strtoul() returns 0 if conversion fails.
        unsigned long ms = strtoul(cmd.c_str(), nullptr, 10);
        delay(ms);
        return true;
      }
    
    default:
      if (cmd.length() >= 1 && (cmd[0] == '0' || cmd[0] == '1'))
      {
        writeOutputs(to16(cmd));      // to16() returns 0 if conversion fails
        return true;
      }
      else
      {
        Serial.println("ERROR      : Type a BIN or a HEX starting with 0x");
        return false;
      }
  }
}

void doSerialRead()
{
  String msg, cmd;
  msg = Serial.readStringUntil('\n'); // function removes '\n' from serial buffer and does not return a '\n'
  msg.trim();                         // remove CR if terminal is sending one
  if (msg.length() == 0)              // if just pressing ENTER
  {
    Serial.println();
    printOutputs();
    readInputs();
    printInputs();
    return;
  }

  // Display Help
  if (msg.length() > 0 && msg[0] == '?')
  {
    Serial.println();
    printCmds();
    return;
  }

  // Display Command(s)
  Serial.println();
  Serial.print("COMMAND(s) : ");
  Serial.println(msg);

  // Parse Command(s)
  while (msg.length() > 0)
  {
    int idx = msg.indexOf(' ');       // find space
    if (idx == 0)
    {
      msg.remove(0, 1);               // remove leading space
      continue;                       // and jump to while()
    }
    else if (idx > 0)
      cmd = msg.substring(0, idx);    // extract command
    else
      cmd = msg;                      // it's the last command
    msg.remove(0, cmd.length());      // remove command from msg
    if (!parseCmd(cmd))               // call last as it can alter cmd
      break;                          // on error exit loop
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
  delayMicroseconds(SIG_SETTLE_US);

  // Print Help, Outputs and Inputs
  printCmds();
  Serial.println();
  printOutputs();
  readInputs(); // do a first read to init g_in
  printInputs();
}

void loop()
{
  if (Serial.available())
    doSerialRead();
}
