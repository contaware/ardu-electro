/* 
  Read/Write SPI Flash chips from the W25Qxxx/S25FLxxx series

  - The W25Qxxx/S25FLxxx operate at 2.7V - 3.6V.

  - These Flashes have a specified life of 100'000 write/erase cycles.

  - Connect the chip to the Arduino like:
                 ----u----
             CS |1       8| VCC <-> 3.3V
           MISO |2       7| HOLD <-> 3.3V
    3.3V <-> WP |3       6| SCK
            GND |4       5| MOSI
                 ---------
             W25Qxxx/S25FLxxx

  - If the WP (write protect) pin is LOW, then the status register cannot be 
    written.
  
  - The HOLD pin allows the device to be paused while it is actively selected. 
    When HOLD is brought LOW, while CS is LOW, the MISO pin will be at high 
    impedance and signals on the MOSI and SCK pins will be ignored.

  - The SerialFlash.h library works like:

    1. Files are created with a fixed size which can never change or grow. 
  
    2. Once created, files cannot be renamed or deleted, filenames can be 
       released with SerialFlash.remove(), but the actual space used by the 
       file is not freed! However, a new file with the released filename may 
       be created.
  
    3. Files begin with all bytes erased (255). Each byte may be written only 
       once. Files created as erasable may be fully erased to allow new data 
       to be written, note that those files have their size rounded up to the 
       nearest number of erasable blocks.
  
    4. Best performance is achieved by writing in 256 byte chunks, though 
       individual bytes may be written.

    5. Many library functions become blocking when they have to wait that the 
       Flash finishes the previous operation (like writing or erasing). 
       To avoid being blocked, poll the Flash with SerialFlash.ready().
*/
#include <SerialFlash.h> // https://github.com/PaulStoffregen/SerialFlash
#include <SPI.h>

#define CS_PIN      6

uint8_t randomBuf1[3];
uint8_t randomBuf2[3];
unsigned char buf[16];
char filename[64];

void printHEX(uint8_t b)
{
  if (b < 16)
    Serial.print("0");
  Serial.print(b, HEX);
}

void printBuf(uint8_t* buf, int bufSize)
{
  for (int i = 0; i < bufSize ; i++)
  {
    printHEX(buf[i]);
    if (i < bufSize - 1)
      Serial.print(' ');
  }
}

void printFiles()
{
  uint32_t filesize;
  SerialFlash.opendir();
  while (SerialFlash.readdir(filename, sizeof(filename), filesize))
  {
    Serial.print(filename);
    Serial.print(" (");
    Serial.print(filesize);
    Serial.print(" bytes): ");
    SerialFlashFile file = SerialFlash.open(filename);
    if (file)
    {
      filesize = min(filesize, sizeof(buf));
      file.read(buf, filesize);
      printBuf(buf, filesize);
    }
    Serial.println();
  }
}

bool getChipInfo()
{
  SerialFlash.readID(buf);
  Serial.print("JEDEC ID:    ");
  printBuf(buf, 3); Serial.println();
  Serial.print("Memory Size: ");
  unsigned long chipsize = SerialFlash.capacity(buf);
  Serial.print(chipsize);
  Serial.println(" bytes");
  if (chipsize == 0)
    return false;
  Serial.print("Block Size:  ");
  unsigned long blocksize = SerialFlash.blockSize();
  Serial.print(blocksize);
  Serial.println(" bytes");
  return true;
}

void eraseEverything()
{
  Serial.print("Erasing Flash Memory");
  SerialFlash.eraseAll();
  unsigned long prevMillis = millis();
  while (!SerialFlash.ready())
  {
    unsigned long currentMillis = millis();
    if (currentMillis - prevMillis > 2000)
    {
      prevMillis = currentMillis;
      Serial.print(".");
    }
  }
  Serial.println();
}

void setup()
{
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.println("SPI Flash Test");

  // Init random bufs
  randomSeed(analogRead(A0));
  for (unsigned int i = 0; i < sizeof(randomBuf1) ; i++)
    randomBuf1[i] = random(0, 256);
  for (unsigned int i = 0; i < sizeof(randomBuf2) ; i++)
    randomBuf2[i] = random(0, 256);

  // Serial Flash begin
  if (!SerialFlash.begin(CS_PIN))
  {
    Serial.println("ERROR: unable to access the SPI Flash chip!");
    while (true);
  }

  // Get chip information
  if (!getChipInfo())
  {
    Serial.println("ERROR: cannot identify the SPI Flash chip!");
    while (true);
  }

  // Erase everything
  // Note: run this before using this chip for files.
  eraseEverything();

  // Create files (filenames are case sensitive)
  Serial.println("1. Create file1.txt and file2.txt (erasable)");
  if (!SerialFlash.create("file1.txt", sizeof(randomBuf1)))
    Serial.println("Failed to create file1.txt");
  if (!SerialFlash.createErasable("file2.txt", sizeof(randomBuf2)))
    Serial.println("Failed to create file2.txt");

  // Fill the files with random data
  Serial.print("2. Write ");
  printBuf(randomBuf1, sizeof(randomBuf1));
  Serial.print(" to file1.txt and ");
  printBuf(randomBuf2, sizeof(randomBuf2));
  Serial.println(" to file2.txt");
  SerialFlashFile file1 = SerialFlash.open("file1.txt");
  if (file1)
    file1.write(randomBuf1, sizeof(randomBuf1));
  else
    Serial.println("Failed to open file1.txt");
  SerialFlashFile file2 = SerialFlash.open("file2.txt");
  if (file2)
    file2.write(randomBuf2, sizeof(randomBuf2));
  else
    Serial.println("Failed to open file2.txt");
  if (file1 || file2)
    printFiles();

  // Write file2.txt again
  Serial.print("4. Write ");
  printBuf(randomBuf1, sizeof(randomBuf1));
  Serial.println(" to file2.txt after erasing it");
  file2 = SerialFlash.open("file2.txt");
  if (file2)
  {
    file2.erase();
    file2.write(randomBuf1, sizeof(randomBuf1));
    printFiles();
  }
  else
    Serial.println("Failed to open file2.txt");
  
  // Seek and position
  Serial.println("5. Seek file2.txt");
  file2 = SerialFlash.open("file2.txt");
  if (file2)
  {
    // Go to position 1
    file2.seek(1);
    Serial.print("at position ");
    Serial.print(file2.position());
    Serial.print(" we have ");

    // Read at position 1
    uint8_t b;
    file2.read(&b, 1); // position is increased after read 
    printHEX(b);
    Serial.println();
  }
  else
    Serial.println("Failed to open file2.txt");
}

void loop()
{

}
