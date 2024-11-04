/*
  Strings in flash memory (to reduce SRAM memory consumption)

  Normal string literals exist in both flash and SRAM. To avoid this duplication it's possible to
  instruct the compiler to only store a string in flash memory with PROGMEM. Please first jump to
  the examples in setup(), then you can return here to read the following detailed explanation.

  ******************** ADVANCED EXPLANATION OF THE PSTR("") AND F("") MACROS ********************
  
  From pgmspace.h:
  #define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];}))
  
  Explanation: __c is created in flash and the address of it is returned. Note that this syntax is
  not standard C/C++, it's a GNU C extension called a "statement expression":
  https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
  -pedantic and other options cause warnings for many GNU C extensions, __extension__ around the 
  extension prevents those warnings.

  From WString.h:
  class __FlashStringHelper;
  #define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))
  
  Explanation: PSTR returns a char*, doing a print(const char[]) on it would not work because it
  does not know how to print strings stored in flash. By type-casting we force the compiler to 
  use print(const __FlashStringHelper *) which does know how to print strings stored in flash.
  The class __FlashStringHelper has no body, it's just a dummy type to make the overloading 
  happen, internally print(const __FlashStringHelper *) will cast the pointer back to make it
  work with pgm_read_byte().
*/

// Strings defined globally
// Note: duplicated normal string literals are not using more SRAM and also not more flash, 
// but F("") / PSTR("") strings are always considered different literals even if having the same
// content. Therefore if using the same flash string literal more than once, it makes sense to
// define it as a global PROGMEM variable.
const char ramMsg[] = {"1. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmo"};
const char progmemMsg[] PROGMEM = {"2. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmo"};

void setup()
{
  // Serial Debug
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!

 
  /* STRINGS DEFINED GLOBALLY */
  
  // 1. String gets loaded from flash into SRAM at startup
  Serial.println(ramMsg);

  // 2. If string is in flash
  for (byte k = 0; k < strlen_P(progmemMsg); k++) // there are also strcpy_P and strcat_P
  {
    char myChar = pgm_read_byte(progmemMsg + k);
    Serial.print(myChar);
  }
  Serial.println();
  Serial.println("or:");
  Serial.println((const __FlashStringHelper *)progmemMsg);


  /* STRINGS DEFINED LOCALLY */
  
  // 3. String gets loaded from flash into SRAM at startup
  Serial.println("3. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmo");
  
  // 4. Locally defined variables need the static keyword in order to work with PROGMEM
  static const char localMsg1[] PROGMEM = "4. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmo";
  Serial.println((const __FlashStringHelper *)localMsg1);

  // 5. PSTR() returns const char*
  const char* localMsg2 = PSTR("5. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmo");
  Serial.println((const __FlashStringHelper *)localMsg2);
  
  // 6. PSTR() directly
  Serial.println((const __FlashStringHelper *)PSTR("6. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmo"));
  
  // 7. Illustration of what PSTR() does
  const char* localMsg3;
  {
    static const char __c[] PROGMEM = "7. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmo";
    localMsg3 = &__c[0];  // after this point __c is not visible anymore and thus it will not clash 
  }                       // with other PSTR() calls, but it remains accessible through localMsg3
  Serial.println((const __FlashStringHelper *)localMsg3);
  
  // 8. The F() macro is type-casting PSTR() for us (see example 6.)
  Serial.println(F("8. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmo"));
}

void loop()
{

}
