#include <SPI.h>

#define MODE BYTES

#define CS 10
#define BUFFER_SIZE 32
#define SERIAL_BAUD_RATE 9600
#define SPI_CLK_FREQ_DIV1 16000000
#define SPI_CLK_FREQ_DIV2  8000000
#define SPI_CLK_FREQ_DIV4  4000000
#define SPI_CLK_FREQ_DIV8  2000000
#define SPI_CLK_FREQ_DIV16 1000000
#define SPI_CLK_FREQ_DIV32  500000
#define SPI_CLK_FREQ_DIV64  250000
#define SPI_CLK_FREQ_DIV128 125000


char hex_pair[3] = "";
char buffer[BUFFER_SIZE];
char *c;


void setup()
{
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  Serial.begin(SERIAL_BAUD_RATE);
  SPI.begin();
  while (!Serial) {}
}


void loop()
{
  if (Serial.available() > 0)
  {
    uint8_t Nbytes_read = Serial.readBytesUntil('\n', buffer, BUFFER_SIZE);

    Serial.print(F("Received: "));
    for (int i = 0; i < Nbytes_read; i++)
    {
      Serial.print(buffer[i]);
    }
    Serial.print(F(" (")); Serial.print(Nbytes_read); Serial.print(F(" bytes)"));
    Serial.print('\n');

    #if MODE == HEX
    for (int i = 0; i < Nbytes_read; i++) {
      if (isxdigit(buffer[i]) == 0) {
        Serial.println(F("ERROR: invalid hex string"));
        return;
      }
    }

    if ((Nbytes_read & 1) == 1) {
      Serial.println(F("ERROR: incomplete (odd length) hex string"));
      return;
    }
    #endif

    c = &buffer[0];

    SPI.beginTransaction(SPISettings(SPI_CLK_FREQ_DIV128, MSBFIRST, SPI_MODE0));
    digitalWrite(CS, LOW);
    #if MODE == HEX
    for (int i = 0; i < (Nbytes_read/2); i++) {
      hex_pair[0] = *c;
      hex_pair[1] = *(c+1);
      c = c+2;
      SPI.transfer((char) strtoul(hex_pair, NULL, 16));
    }
    #endif
    #if MODE == BYTES
    for (int i = 0; i < Nbytes_read; i++) {
      SPI.transfer((char) buffer[i]);
    }
    #endif
    digitalWrite(CS, HIGH);
    SPI.endTransaction();
  }
}
