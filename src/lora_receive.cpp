#include <App.hpp>
#include <LoRaWANHandler.hpp>
#include <DisplayHandler.hpp>

void lora_receive(unsigned long rxFrameCounter)
{
#ifdef SERIAL_ON
  SERIAL_PRINT(F("Received "));
  SERIAL_PRINT(LMIC.dataLen);
  SERIAL_PRINTLN(F(" bytes of payload"));
  for (int i = 0; i < LMIC.dataLen; i++)
  {
    SERIAL_PRINTF("%02X ", LMIC.frame[LMIC.dataBeg + i]);
  }
  SERIAL_PRINTLN();
#endif

/*
#ifdef DISPLAY_ON
  for (int i = 0; i < 8 && i < LMIC.dataLen; i++)
  {
    char buf[3];
    sprintf(buf, "%02X", LMIC.frame[LMIC.dataBeg + i]);
    display.drawString(i * 16, 36, buf);
  }
#endif
*/

}
