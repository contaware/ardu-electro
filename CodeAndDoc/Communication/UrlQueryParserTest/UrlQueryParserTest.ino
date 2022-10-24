/*
  Url Query Parser Test
  
  - The functions tested here are used by the Ethernet and WiFi examples.

  - The code tries to use as little RAM as possible.

  - The parser supports query strings in the form param=value separated by ampersands '&'.
    Parameters-only are allowed, either with or without an ending equal character.
*/

static bool getQueryValueFromPair(const String& s, int posStartInclusive, int posEndExclusive, const String& param, String& value)
{
  if (posStartInclusive < posEndExclusive)
  {
    // Split at '='
    int pos;
    if ((pos = s.indexOf('=', posStartInclusive)) >= posStartInclusive && pos < posEndExclusive)
    {
      int len = pos - posStartInclusive;
      if (len == (int)param.length() && memcmp(s.c_str() + posStartInclusive, param.c_str(), len) == 0)
      {
        pos++;                      // skip '='
        if (pos < posEndExclusive)  // at least a char?
          value = s.substring(pos, posEndExclusive);
        else
          value = "";
        return true;
      }
    }
    // param only
    else
    {
      int len = posEndExclusive - posStartInclusive;
      if (len == (int)param.length() && memcmp(s.c_str() + posStartInclusive, param.c_str(), len) == 0)
      {
        value = "";
        return true;
      }
    }
  }

  return false;
}
  
static bool getQueryValue(const String& s, const String& param, String& value)
{
  // Search query init
  int pos;
  if ((pos = s.indexOf('?')) < 0)
    return false;
  else
    pos++;
    
  // Loop pairs
  int ampPos;
  while ((ampPos = s.indexOf('&', pos)) >= pos)
  {
    if (getQueryValueFromPair(s, pos, ampPos, param, value))
      return true;
    pos = ampPos + 1; // next pair
  }

  // Last pair
  if (getQueryValueFromPair(s, pos, s.length(), param, value))
    return true;
  
  return false;
}

static void test(const String& s, const String& param)
{
  String value;
  if (getQueryValue(s, param, value))
  {
    Serial.print(F("  getQueryValue=true  : ->"));
    Serial.print(param);
    Serial.print(F("="));
    Serial.print(value);
    Serial.println(F("<-"));
  }
  else
  {
    Serial.print(F("  getQueryValue=false : ->"));
    Serial.print(param);
    Serial.println(F("<-"));
  }
}

void setup()
{
  // Serial Debug
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!

  Serial.println(F("------------------ Two params -----------------"));
  String sUrl = "/?param1=value1&param2=value2";
  Serial.println(sUrl);
  test(sUrl, "param1");
  test(sUrl, "param2");
  Serial.println();
  
  Serial.println(F("---------- Params are case sensitive ----------"));
  sUrl = "/?param1=value1&Param2=value2";
  Serial.println(sUrl);
  test(sUrl, "param1");
  test(sUrl, "param2");
  Serial.println();

  Serial.println(F("--- Duplicated params -> first one returned ---"));
  sUrl = "/?param1=value1&param1=value2";
  Serial.println(sUrl);
  test(sUrl, "param1");
  Serial.println();
  
  Serial.println(F("----------- Query ending in & is OK -----------"));
  sUrl = "/?p=v&param2=value2&";
  Serial.println(sUrl);
  test(sUrl, "param");
  test(sUrl, "param2");
  test(sUrl, "p");
  Serial.println();
  
  Serial.println(F("------------- Duplicated & are OK -------------"));
  sUrl = "/?param1=value1&&param2=value2&&";
  Serial.println(sUrl);
  test(sUrl, "param1");
  test(sUrl, ""); // returns false
  test(sUrl, "param2");
  Serial.println();
  
  Serial.println(F("----------- Params with empty values ----------"));
  sUrl = "/?param1=&p";
  Serial.println(sUrl);
  test(sUrl, "param1");
  test(sUrl, "p");
  Serial.println();
  
  Serial.println(F("------ Null-param, makes not much sense -------"));
  sUrl = "/?=value";
  Serial.println(sUrl);
  test(sUrl, "");
  Serial.println();
}

void loop()
{
  
}
