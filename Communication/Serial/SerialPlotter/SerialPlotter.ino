/*
  Serial Plotter

  - The vertical Y axis auto adjusts as the value of the output increases 
    or decreases, and the X axis is fixed at 500 points in IDE 1.x and at
    50 points in IDE 2.x. When reaching the X axis limit the oldest points 
    are dropped, the visual effect is the graph scrolling out to the left.
    
  - Each tick of the axis equals to an executed serial println() command,
    there is no time scale possibility.

  - To plot multiple values do separate them by a space, comma or \t. 

  - To print labels prepend the values with a label text followed by a 
    colon. Cannot use colon, space, comma or \t in label text.
*/

const int halfPeriod = 70;          // number of sample for a half-period
const bool noYAutoScaling = false;  // set to true to avoid auto-scaling Y axis

void setup()
{
  // Init Serial
  Serial.begin(9600);
}

void loop()
{
  static int count = 0;
  static int addValue = 1;

  // Print values
  if (noYAutoScaling)
  {
    Serial.print("Max:");
    Serial.print(halfPeriod);
    Serial.print(",");
  }
  Serial.print("My-Label1:");
  Serial.print(count);
  Serial.print(",");
  Serial.print("My-Label2:");
  Serial.print(-count);
  if (noYAutoScaling)
  {
    Serial.print(",Min:");
    Serial.print(-halfPeriod);
  }
  Serial.println();
  
  // Inc/Dec
  count += addValue;

  // What to do in next round
  if (count >= halfPeriod)
    addValue = -1;
  else if (count <= 0)
    addValue = 1;

  // Delay
  delay(80);
}
