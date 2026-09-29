/*
  Digital Testing Board to verify digital chips with Arduino

  - The chip to be verified is called DUT (device under test).

  - This project has 10 outputs and 8 inputs.
    * The last output is connected to the built-in LED and 
      can also be connected to the DUT without any problems.
    * This tester is simple, it does not use any interrupts.

  - I implemented the tester making an Arduino UNO shield with the 
    8 inputs buffered through a 74AHCT244 to support DUTs with TTL 
    outputs, such as the 74LSxxx chip family:
    Shield IN         74AHCT244             Arduino UNO
    IN0     ->     pin2  |> pin18    ->    11
    IN1     ->     pin4  |> pin16    ->    12 
    IN2     ->     pin6  |> pin14    ->    A0 
    IN3     ->     pin8  |> pin12    ->    A1 
    IN4     ->     pin17 |> pin3     ->    A2 
    IN5     ->     pin15 |> pin5     ->    A3 
    IN6     ->     pin13 |> pin7     ->    A4 
    IN7     ->     pin11 |> pin9     ->    A5 
    Warning: to prevent floating 74AHCT244 inputs use 100kΩ pull-downs.
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

// Choose a Digits Separator
// "":   no separator
// "_":  most programming languages
// "'":  C/C++ and Switzerland
// ",":  most English-speaking countries
// ".":  many non-English speaking countries
// Attention: do not set to " " because the space is used to separate commands.
const char DIGITS_SEP[] = "'";

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

// Forward declarations for functions with default arguments
void printInputs(uint8_t changedBits = 0);

void printCmds()
{
  Serial.println("Type space-separated commands in upper window and press ENTER:");
  Serial.println("?          : Display this help");
  Serial.println("ENTER or S : Show outputs and inputs");
  Serial.println("T0..T9     : Toggle given output");
  Serial.print("P0..P9     : Pulse given output");
  Serial.print(" (");
  Serial.print(SIG_SETTLE_US + PULSE_HOLD_US);
  Serial.println("us)");
  Serial.println("value      : Set outputs 9..0 to BIN or HEX starting with 0x");
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

void toggleOutput(int outNum)
{
  outNum = constrain(outNum, 0, TESTER_OUT_LAST);
  int outPin = TesterOutToPin(outNum);

  // Read current output value to decide the toggle direction
  bool changeToHigh = !bitRead(g_out, outNum);

  // Change Output and let it settle
  digitalWrite(outPin, changeToHigh ? HIGH : LOW);
  delayMicroseconds(SIG_SETTLE_US);

  // Update output variable
  bitWrite(g_out, outNum, changeToHigh ? 1 : 0);
}

uint8_t pulseOutput(int outNum)
{
  outNum = constrain(outNum, 0, TESTER_OUT_LAST);
  int outPin = TesterOutToPin(outNum);

  // Read current output value to decide the pulse direction
  bool highPulse = !bitRead(g_out, outNum);

  // Pulse Output and let it settle
  digitalWrite(outPin, highPulse ? HIGH : LOW);
  delayMicroseconds(SIG_SETTLE_US);

  // Leave Output unchanged for PULSE_HOLD_US and, in the meantime, read Inputs
  unsigned long startTime = micros();
  uint8_t changedBits = readInputs();
  unsigned long elapsedTime = micros() - startTime; 
  if (elapsedTime < PULSE_HOLD_US)
    delayMicroseconds(PULSE_HOLD_US - elapsedTime);

  // Return Output to its initial state and let it settle
  digitalWrite(outPin, highPulse ? LOW : HIGH);
  delayMicroseconds(SIG_SETTLE_US);

  return changedBits;
}

void writeOutputs(uint16_t outValue)
{
  const uint16_t maxValue = (1U << (TESTER_OUT_LAST + 1)) - 1U;
  outValue = constrain(outValue, 0, maxValue);

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

  // Update output variable
  g_out = outValue;
}

void printOutputs(uint16_t out)
{
  Serial.print("OUT[9..0]  : ");
  for (int i = 9 ; i >= 0 ; i--)
  {
    if (i == 7 || i == 3) Serial.print(DIGITS_SEP);
    Serial.print(bitRead(out, i));
  }
  Serial.print(" (0x");
  if (out < 0x100) Serial.print('0');
  if (out < 0x10) Serial.print('0');
  Serial.print(out, HEX);
  Serial.println(")");
}

uint8_t readInputs()
{
  // Read the Inputs
  uint8_t in = 0;
  bitWrite(in, 7, digitalRead(TESTER_IN7_PIN));
  bitWrite(in, 6, digitalRead(TESTER_IN6_PIN));
  bitWrite(in, 5, digitalRead(TESTER_IN5_PIN));
  bitWrite(in, 4, digitalRead(TESTER_IN4_PIN));
  bitWrite(in, 3, digitalRead(TESTER_IN3_PIN));
  bitWrite(in, 2, digitalRead(TESTER_IN2_PIN));
  bitWrite(in, 1, digitalRead(TESTER_IN1_PIN));
  bitWrite(in, 0, digitalRead(TESTER_IN0_PIN));

  // Set bits that changed
  uint8_t changedBits = in ^ g_in; 

  // Update global variable
  g_in = in;

  return changedBits;
}

void printDigitsSepSpace()
{
  for (size_t s = 0 ; s < strlen(DIGITS_SEP) ; s++)
    Serial.print(" ");
}

void printInputs(uint8_t changedBits/*=0*/)
{
  // Print inputs
  Serial.print("IN[7..0]   : ");
  Serial.print("  ");    // space of OUT9, OUT8
  printDigitsSepSpace(); // space of digits separator
  for (int i = 7 ; i >= 0 ; i--)
  {
    if (i == 3) Serial.print(DIGITS_SEP);
    Serial.print(bitRead(g_in, i));
  }
  Serial.print(" (0x");
  if (g_in < 0x10) Serial.print('0');
  Serial.print(g_in, HEX);
  Serial.println(")");

  // Mark changed inputs
  if (changedBits)
  {
    Serial.print("             ");
    Serial.print("  ");    // space of OUT9, OUT8
    printDigitsSepSpace(); // space of digits separator
    for (int i = 7 ; i >= 0 ; i--)
    {
      if (i == 3) printDigitsSepSpace();
      Serial.print(bitRead(changedBits, i) ? "^" : " ");
    }
    Serial.println();
  }
}

uint16_t to16(String msg)
{
  // Remove digits separator
  msg.replace(DIGITS_SEP, "");

  // Convert given String to a uint16_t
  // Note: strtol() returns 0 if conversion fails.
  long outValue;
  if (msg.length() >= 3 && (msg[0] == '0' && tolower(msg[1]) == 'x'))
    outValue = strtol(msg.c_str(), nullptr, 16); // HEX
  else
    outValue = strtol(msg.c_str(), nullptr, 2);  // BIN
  return (uint16_t)constrain(outValue, 0L, 65535L);
}

void printCmd(const String& cmd)
{
  Serial.println();
  Serial.print("COMMAND    : ");
  Serial.println(cmd);
}

bool parseCmd(String& cmd)
{
  switch (toupper(cmd[0]))
  {
    case 'T':
      printCmd(cmd);
      if (cmd.length() >= 2 && isdigit(cmd[1]))
      {
        cmd.remove(0, 1);             // remove 'T' char
        int outNum = cmd.toInt();     // returns 0 if conversion fails
        readInputs();                 // read Inputs before the Change
        toggleOutput(outNum);         // toggle given Output
        printOutputs(g_out);          // print all Outputs
        printInputs(readInputs());    // print all Inputs marking Changes
        return true;
      }
      else
      {
        Serial.println("ERROR      : After 'T' type an output number");
        return false;
      }

    case 'P':
      printCmd(cmd);
      if (cmd.length() >= 2 && isdigit(cmd[1]))
      {
        cmd.remove(0, 1);             // remove 'P' char
        int outNum = cmd.toInt();     // returns 0 if conversion fails
        readInputs();                 // read Inputs before the Pulse
        uint8_t changedBits = pulseOutput(outNum);
        uint16_t out = g_out;
        bitWrite(out, outNum, !bitRead(g_out, outNum));
        printOutputs(out);            // print all Outputs during pulse
        printInputs(changedBits);     // print all Inputs during pulse marking Changes
        printOutputs(g_out);          // print all Outputs after pulse
        printInputs(readInputs());    // print all Inputs after pulse marking Changes
        return true;
      }
      else
      {
        Serial.println("ERROR      : After 'P' type an output number");
        return false;
      }

    case 'S':
      printCmd(cmd);
      printOutputs(g_out);
      readInputs();
      printInputs();
      return true;

    case 'W':
      if (cmd.length() == 1)
        delay(1000);
      else
      {
        cmd.remove(0, 1);             // remove 'W' char
        // Convert given String to an unsigned long
        // Note: strtoul() returns 0 if conversion fails.
        unsigned long ms = strtoul(cmd.c_str(), nullptr, 10);
        delay(ms);
      }
      return true;

    case '?':
      Serial.println();
      printCmds();
      return true;
    
    default:
      printCmd(cmd);
      if (cmd.length() >= 1 && (cmd[0] == '0' || cmd[0] == '1'))
      {
        readInputs();                 // read Inputs before the Change
        writeOutputs(to16(cmd));      // to16() returns 0 if conversion fails
        printOutputs(g_out);          // print all Outputs
        printInputs(readInputs());    // print all Inputs marking Changes
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
    printOutputs(g_out);
    readInputs();
    printInputs();
    return;
  }

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
      break;                          // exit loop
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
  printOutputs(g_out);
  readInputs(); // do a first read to init g_in
  printInputs();
}

void loop()
{
  if (Serial.available())
    doSerialRead();
}
