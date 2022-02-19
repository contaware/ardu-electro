/*
  Note that this does not take into account dynamic memory allocation (ie. with malloc or new) 
  however as a guide to how much memory you have at the beginning of setup (before your code 
  actually does anything) this could be very useful.
*/
size_t freeRam()
{
  return RAMEND - size_t(__malloc_heap_start);
}

const char ramMsg[] = {"3. Lorem Ipsum is simply dummy text of the printing and typesetting industry"};
const char progmemMsg[] PROGMEM = {"4. Lorem Ipsum is simply dummy text of the printing and typesetting industry"};

void setup()
{
  Serial.begin(9600);

  // 1. This puts the string in RAM
  Serial.println("1. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmo");

  // 2. The F() macro tells the compiler to leave this particular array in PROGMEM and instructs the compiler
  //    to use print(const __FlashStringHelper *) which does know how to print strings stored in Flash
  Serial.println(F("2. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmo"));

  // 3. Normal message from RAM
  Serial.println(ramMsg);

  // 4. That's necessary if string is in program memory
  for (byte k = 0; k < strlen_P(progmemMsg); k++) // there are also strcpy_P and strcat_P
  {
    char myChar = pgm_read_byte_near(progmemMsg + k);
    Serial.print(myChar);
  }
  Serial.println();

  // Display the free memory
  Serial.print(F("Free memory = "));
  Serial.println(freeRam());
}

void loop()
{

}
