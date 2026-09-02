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

// To be compatible with all platforms keep track of the output values
// bit0 = OUT0
// ...
// bit7 = OUT7
//
// bit8 = OUT8
// bit9 = OUT9 (LED)
uint16_t g_out = 0;

// Polling of the Tester inputs
bool g_pollInputs = true;
const unsigned long POLL_TIME_MS = 1000;
unsigned long g_lastPollInputsMs;
uint8_t g_in = 0;

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
  Serial.println("value      : Set outputs 7..0 to value");
  Serial.println("ENTER      : Show tester outputs and inputs");
  Serial.println("I          : Input changes detection ON/OFF");
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

void writeOutputs8(int outValue)
{
  outValue = constrain(outValue, 0, 255);
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
  Serial.print(lowB, HEX);
  Serial.print(" ");
  Serial.print(lowB);
  Serial.print(") , ");

  Serial.print("OUT[9]: ");
  Serial.print(bitRead(g_out, 9));
  Serial.print(" , OUT[8]: ");
  Serial.println(bitRead(g_out, 8));
}

bool readInputs()
{
  // Read the 8 Inputs
  uint8_t in = 0;
  bitWrite(in, 7, digitalRead(TESTER_IN7_PIN));
  bitWrite(in, 6, digitalRead(TESTER_IN6_PIN));
  bitWrite(in, 5, digitalRead(TESTER_IN5_PIN));
  bitWrite(in, 4, digitalRead(TESTER_IN4_PIN));
  bitWrite(in, 3, digitalRead(TESTER_IN3_PIN));
  bitWrite(in, 2, digitalRead(TESTER_IN2_PIN));
  bitWrite(in, 1, digitalRead(TESTER_IN1_PIN));
  bitWrite(in, 0, digitalRead(TESTER_IN0_PIN));

  // Check whether at least one of the 8 Inputs changed
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
  Serial.print(g_in, HEX);
  Serial.print(" ");
  Serial.print(g_in);
  Serial.println(")");
}

void doSerialRead()
{
  String msg;
  msg = Serial.readStringUntil('\n'); // function removes '\n' from serial buffer and does not return a '\n'
  msg.trim();                         // removes also the CR if terminal is sending one
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
      if (msg.length() >= 2)
      {
        msg.remove(0, 1);             // remove 'H' char
        int outNum = msg.toInt();     // returns 0 if conversion fails
        writeOutput(outNum, 1);
      }
      break;

    case 'L':
      if (msg.length() >= 2)
      {
        msg.remove(0, 1);             // remove 'L' char
        int outNum = msg.toInt();     // returns 0 if conversion fails
        writeOutput(outNum, 0);
      }
      break;

    case 'P':
      if (msg.length() >= 2)
      {
        msg.remove(0, 1);             // remove 'P' char
        int outNum = msg.toInt();     // returns 0 if conversion fails
        pulseOutput(outNum);
      }
      break;

    case 'I':
      g_pollInputs = !g_pollInputs;
      if (g_pollInputs)
        g_lastPollInputsMs = millis() - POLL_TIME_MS;
      break;
    
    default:
    {
      // Remove optional '+' char
      if (msg.length() >= 1 && msg[0] == '+')
      {
        msg.remove(0, 1);             // remove '+' char
        msg.trim();                   // trim whitespaces
      }

      // Get numeric value
      if (msg.length() >= 1 && isxdigit(msg[0]))
      {
        // Digits statistics
        bool onlyDigits = true;
        bool onlyBinDigits = true;
        for (unsigned int i = 0 ; i < msg.length(); i++)
        {
          if (!isdigit(msg[i]))
          {
            onlyBinDigits = onlyDigits = false;
            break;
          }
        }
        if (onlyDigits)
        {
          for (unsigned int i = 0 ; i < msg.length(); i++)
          {
            if (msg[i] != '0' && msg[i] != '1')
            {
              onlyBinDigits = false;
              break;
            }
          }
        }

        // Parse value
        // Note: strtol() and .toInt() return 0 if conversion fails.
        int outValue;
        if (onlyDigits)
        {
          // Consider a binary if it has a length of 4 or more
          if (msg.length() >= 4 && onlyBinDigits)
            outValue = strtol(msg.c_str(), nullptr, 2); // BIN
          else
            outValue = msg.toInt(); // DEC
        }
        else
          outValue = strtol(msg.c_str(), nullptr, 16); // HEX

        // Write value and display outputs
        writeOutputs8(outValue);
        printOutputs();
      }
      else
        Serial.println("ERROR      : Type a valid unsigned number");
      break;
    }
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

  // Init the g_lastPollInputsMs variable
  g_lastPollInputsMs = millis() - POLL_TIME_MS;

  // Print Help, Outputs and Inputs
  printCmds();
  printOutputs();
  readInputs(); // first read inits g_in
  printInputs();
}

void loop()
{
  if (Serial.available())
    doSerialRead();

  if (g_pollInputs)
  {
    unsigned long currentMs = millis();
    if ((currentMs - g_lastPollInputsMs) >= POLL_TIME_MS)
    {
      g_lastPollInputsMs = currentMs;
      if (readInputs())
        printInputs();
    }
  }
}
