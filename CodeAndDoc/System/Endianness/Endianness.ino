#include <limits.h>
#include <stdint.h>

#if CHAR_BIT != 8
#error "unsupported char size"
#endif

enum
{
    O32_LITTLE_ENDIAN = 0x03020100ul,
    O32_BIG_ENDIAN = 0x00010203ul,
    O32_PDP_ENDIAN = 0x01000302ul
};

static const union { unsigned char bytes[4]; uint32_t value; } o32_host_order =
    { { 0, 1, 2, 3 } };

#define O32_HOST_ORDER (o32_host_order.value)

void setup() 
{
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  if ( O32_HOST_ORDER == O32_LITTLE_ENDIAN )
  {
    Serial.println( F( "Little Endian" ) );
  }
  else if ( O32_HOST_ORDER == O32_BIG_ENDIAN )
  {
    Serial.println( F( "Big Endian" ) );
  }
  else if ( O32_HOST_ORDER == O32_PDP_ENDIAN )
  {
    Serial.println( F( "PDP Endian" ) );
  }
  else
  {
    Serial.println( F( "Other Endian" ) );
  }
}

void loop()
{

}
