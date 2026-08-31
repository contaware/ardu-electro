/*
  Digital Testing Board to verify digital chips with Arduino

  - The chip to be verified is called DUT (device under test).

  - This project implements 8 inputs and 10 outputs.
    Note: the last output is connected to the built-in LED and 
          can also be connected to the DUT without any problems.

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

// To be compatible with all platforms keep track of the output values
// bit0 = OUT0
// ...
// bit7 = OUT7
//
// bit8 = OUT8
// bit9 = OUT9 (LED)
uint16_t g_out = 0;

// Polling of the Tester inputs
bool g_pollInputs = false;
const unsigned long POLL_TIME_MS = 1000;
unsigned long g_lastPollInputsMs;

void printCmds()
{
  Serial.println("Type in upper window and press ENTER:");
  Serial.println("out=value  : Set given out (0,1,..,9) to value");
  Serial.println("value      : Set outputs 7..0 to value");
  Serial.println("H or ?     : Show this help");
  Serial.println("I or ENTER : Read tester inputs now");
  Serial.println("P          : Toggle polling tester inputs");
  Serial.println("O          : Show all tester output values");
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
  bitWrite(g_out, outNum, outValue);
  digitalWrite(outPin, outValue ? HIGH : LOW);
}

void writeOutputs8(int outValue)
{
  outValue = constrain(outValue, 0, 255);
  g_out = (g_out & ~255U) | (uint16_t)outValue;
  digitalWrite(TESTER_OUT0_PIN, bitRead(outValue, 0) ? HIGH : LOW);
  digitalWrite(TESTER_OUT1_PIN, bitRead(outValue, 1) ? HIGH : LOW);
  digitalWrite(TESTER_OUT2_PIN, bitRead(outValue, 2) ? HIGH : LOW);
  digitalWrite(TESTER_OUT3_PIN, bitRead(outValue, 3) ? HIGH : LOW);
  digitalWrite(TESTER_OUT4_PIN, bitRead(outValue, 4) ? HIGH : LOW);
  digitalWrite(TESTER_OUT5_PIN, bitRead(outValue, 5) ? HIGH : LOW);
  digitalWrite(TESTER_OUT6_PIN, bitRead(outValue, 6) ? HIGH : LOW);
  digitalWrite(TESTER_OUT7_PIN, bitRead(outValue, 7) ? HIGH : LOW);
}

void printInputs()
{
  uint8_t in = 0;
  bitWrite(in, 7, digitalRead(TESTER_IN7_PIN));
  bitWrite(in, 6, digitalRead(TESTER_IN6_PIN));
  bitWrite(in, 5, digitalRead(TESTER_IN5_PIN));
  bitWrite(in, 4, digitalRead(TESTER_IN4_PIN));
  bitWrite(in, 3, digitalRead(TESTER_IN3_PIN));
  bitWrite(in, 2, digitalRead(TESTER_IN2_PIN));
  bitWrite(in, 1, digitalRead(TESTER_IN1_PIN));
  bitWrite(in, 0, digitalRead(TESTER_IN0_PIN));

  Serial.print("IN[7..0]   : ");
  for (int i = 7 ; i >= 0 ; i--)
  {
    if (i == 3) Serial.print(" ");
    Serial.print(bitRead(in, i));
  }
  Serial.print(" (0x");
  Serial.print(in, HEX);
  Serial.print(" ");
  Serial.print(in);
  Serial.println(")");
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

  Serial.print("OUT9: ");
  Serial.print(bitRead(g_out, 9));
  Serial.print(" , OUT8: ");
  Serial.println(bitRead(g_out, 8));
}

void doSerialRead()
{
  String msg;
  msg = Serial.readStringUntil('\n'); // function removes '\n' from serial buffer and does not return a '\n'
  msg.trim();                         // removes also the CR if terminal is sending one
  if (msg.length() == 0)              // if just pressing ENTER
  {
    printInputs();
    return;
  }

  switch (toupper(msg[0]))
  {
    // ATTENTION: do not use 'A', 'B', 'C', 'D', 'E', 'F'
    //            as commands because they are for hex values!
    case 'H':
    case '?':
      printCmds();   
      break;

    case 'I':
      printInputs();
      break;

    case 'P':
      g_pollInputs = !g_pollInputs;
      if (g_pollInputs)
        g_lastPollInputsMs = millis() - POLL_TIME_MS;
      break;

    case 'O':
      printOutputs();
      break;
    
    default:
    {
      // With the equal sign we output a single bit
      int idx = msg.indexOf('=');
      if (idx >= 1)
      {
        // Get output number
        String sOutNum = msg.substring(0, idx); // ending index is exclusive
        int outNum = sOutNum.toInt();

        // Get value
        msg = msg.substring(idx);
        msg.remove(0, 1);             // remove '=' char
        msg.trim();                   // trim whitespaces
        if (msg.length() >= 1)
        {
          writeOutput(outNum, msg.toInt());
          printOutputs();
        }
        else
          Serial.println("ERROR      : Specify a value after '='");
        break;
      }

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

        // Write value and display the 8 lower bits
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

  // Print Help
  printCmds();
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
      printInputs();
    }
  }
}
