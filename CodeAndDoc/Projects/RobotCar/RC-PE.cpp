#include "RC-PE.h"

#if USE_PHOTO_ENC == true

#include "RC-LCD.h"

volatile unsigned long photoEncLastMeasureMs;
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
  unsigned long currentMs = millis();
  photoEncLastMeasureMs = currentMs; 
  photoEncLeftChanges = 0;
  photoEncRightChanges = 0;
  photoEncLeftLastChanges = 0;
  photoEncRightLastChanges = 0;
  photoEncLeftDebounceMs = currentMs - PHOTO_ENC_DEBOUNCE_TIME_MS; // this correctly wraps back and assures that the ISR is called with the first change
  photoEncRightDebounceMs = photoEncLeftDebounceMs;
  g_photoEncLeftCountdown = 0;
  g_photoEncRightCountdown = 0;
  attachInterrupt(digitalPinToInterrupt(PHOTO_ENC_LEFT_PIN), photoEncLeftISR, CHANGE);   // use CHANGE and not FALLING because the bounces on both edges are triggering FALLING
  attachInterrupt(digitalPinToInterrupt(PHOTO_ENC_RIGHT_PIN), photoEncRightISR, CHANGE); // use CHANGE and not FALLING because the bounces on both edges are triggering FALLING
  g_photoEncTimer.begin(300, photoEncMeasure);
}

void photoEncMeasure()
{
  // Read
  byte currentLeftChanges = photoEncLeftChanges;
  byte currentRightChanges = photoEncRightChanges;
  unsigned long currentMs = millis();

  // Calc. diff
  byte diffLeft = currentLeftChanges - photoEncLeftLastChanges;
  byte diffRight = currentRightChanges - photoEncRightLastChanges;
  unsigned long diffMs = currentMs - photoEncLastMeasureMs;
  if (diffMs == 0) diffMs = 1; // avoid division by zero

  // Calc. rpm and speed
  unsigned long rpmLeft = (60 * 1000UL / PHOTO_ENC_CHANGES_PER_TURN) * (unsigned long)diffLeft / diffMs;
  unsigned long rpmRight = (60 * 1000UL / PHOTO_ENC_CHANGES_PER_TURN) * (unsigned long)diffRight / diffMs;
  unsigned long rpmAvg = (rpmLeft + rpmRight) / 2;
  double speedAvg = rpmAvg * PHOTO_ENC_CARWHEEL_CIRCUMFERENCE_MM / 60.0 / 1000.0;

  // Display rpm and speed
#if USE_LCD == true
  displayRpmSpeed(rpmAvg, speedAvg);
#endif

  // Debug print
  if (rpmAvg > 0)
  {
    DPRINT(F("HC-020K "));
    DPRINT(rpmAvg);
    DPRINT(F("rpm "));
    DPRINT(speedAvg);
    DPRINTLN(F("m/s"));
  }

  // Update vars
  photoEncLeftLastChanges = currentLeftChanges;
  photoEncRightLastChanges = currentRightChanges;
  photoEncLastMeasureMs = currentMs;
}

#endif
