void setup()
{ 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, INPUT_PULLUP); // the following is also working, but not so understandable: pinMode(2, INPUT); digitalWrite(2, HIGH);
} 

void loop() 
{  
  int digitalVal = digitalRead(2);
  if (HIGH == digitalVal)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}
