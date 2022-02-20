#include "RC-PE.h"

#if USE_PHOTO_ENC == 1

#include "RC-LCD.h"

volatile byte photoEncLeftChanges;
volatile byte photoEncLeftLastChanges;
volatile unsigned long photoEncLeftDebounceMs;
volatile byte photoEncRightChanges;
volatile byte photoEncRightLastChanges;
volatile unsigned long photoEncRightDebounceMs;

volatile byte g_photoEncLeftCountdown;
volatile byte g_photoEncRightCountdown;
TimerPoll g_photoEncTimer;

// To debounce we could also use micros() because a debounce of 100 us is far enough 
// according to my oscilloscope tests. But micros() is slower than millis() which 
// just reads the milliseconds counter.

void photoEncLeftISR() // called on falling and rising edges, see PHOTO_ENC_CHANGES_PER_TURN
{
  unsigned long currentMs = millis();
  if ((currentMs - photoEncLeftDebounceMs) >= PHOTO_ENC_DEBOUNCE_TIME_MS)
  {
    photoEncLeftDebounceMs = currentMs;
    photoEncLeftChanges++;
    if (g_photoEncLeftCountdown > 0)
      --g_photoEncLeftCountdown;
  }
}

void photoEncRightISR() // called on falling and rising edges, see PHOTO_ENC_CHANGES_PER_TURN
{
  unsigned long currentMs = millis();
  if ((currentMs - photoEncRightDebounceMs) >= PHOTO_ENC_DEBOUNCE_TIME_MS)
  {
    photoEncRightDebounceMs = currentMs;
    photoEncRightChanges++;
    if (g_photoEncRightCountdown > 0) 
      --g_photoEncRightCountdown;
  }
}

void photoEncBegin()
{
  pinMode(PHOTO_ENC_LEFT_PIN, INPUT);
  pinMode(PHOTO_ENC_RIGHT_PIN, INPUT);
  photoEncLeftChanges = 0;
  photoEncRightChanges = 0;
  photoEncLeftLastChanges = 0;
  photoEncRightLastChanges = 0;
  photoEncLeftDebounceMs = millis() - PHOTO_ENC_DEBOUNCE_TIME_MS; // this correctly wraps back and assures that the ISR is called with the first change
  photoEncRightDebounceMs = photoEncLeftDebounceMs;
  g_photoEncLeftCountdown = 0;
  g_photoEncRightCountdown = 0;
  attachInterrupt(digitalPinToInterrupt(PHOTO_ENC_LEFT_PIN), photoEncLeftISR, CHANGE);   // use CHANGE and not FALLING because the bounces on both edges are triggering FALLING
  attachInterrupt(digitalPinToInterrupt(PHOTO_ENC_RIGHT_PIN), photoEncRightISR, CHANGE); // use CHANGE and not FALLING because the bounces on both edges are triggering FALLING
  g_photoEncTimer.begin(300, photoEncMeasure);
}

void photoEncMeasure(unsigned long elapsedTimeMs)
{
  // Read
  byte currentLeftChanges = photoEncLeftChanges;
  byte currentRightChanges = photoEncRightChanges;

  // Calc. diff
  byte diffLeft = currentLeftChanges - photoEncLeftLastChanges;
  byte diffRight = currentRightChanges - photoEncRightLastChanges;

  // RPM
  unsigned long rpmLeft = (60 * 1000UL / PHOTO_ENC_CHANGES_PER_TURN) * (unsigned long)diffLeft / elapsedTimeMs;
  unsigned long rpmRight = (60 * 1000UL / PHOTO_ENC_CHANGES_PER_TURN) * (unsigned long)diffRight / elapsedTimeMs;
  unsigned long rpmAvg = (rpmLeft + rpmRight) / 2;
  double speedAvg = rpmAvg * PHOTO_ENC_CARWHEEL_CIRCUMFERENCE_MM / 60.0 / 1000.0;
#if USE_LCD == 1
  g_lcd.setCursor(0, 0);
  char s[17];
  sprintf(s, "RPM: L=%03lu R=%03lu", rpmLeft, rpmRight);
  g_lcd.print(s);
  g_lcd.setCursor(0, 1);
  char speedAvgStr[6];
  sprintf(s, "Speed: %s m/s", dtostrf(speedAvg, 5, 2, speedAvgStr));
  g_lcd.print(s);
#endif

  // Update vars
  photoEncLeftLastChanges = currentLeftChanges;
  photoEncRightLastChanges = currentRightChanges;
}

#endif
