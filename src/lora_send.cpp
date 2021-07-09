#include <App.hpp>
#include <LoRaWANHandler.hpp>
#include <DisplayHandler.hpp>

static uint8_t mydata[64];
static char buffer[24];

void lora_send(unsigned long txFrameCounter)
{
#ifdef ADC_PIN
  float bat_sum = 0;
  for (int i = 0; i < NO_BAT_SAMPLES; i++)
  {
    bat_sum += analogRead(ADC_PIN);
  }
  bat_sum /= NO_BAT_SAMPLES;

  // 3.3 / 4095 * 2 =
  float bat = bat_sum * 6.6 / 4095.0;
  SERIAL_PRINTF("bat=%.02fV\n", bat);
  SERIAL_PRINTF("rssi=%d\n", LMIC.rssi);
  sprintf((char *)mydata, "Fcnt=%ld, bat=%.02fV, rssi=%d", txFrameCounter, bat, LMIC.rssi);
#else
  sprintf((char *)mydata, "Fcnt=%ld, rssi=%d", txFrameCounter, LMIC.rssi);
#endif

  // Prepare upstream data transmission at the next possible time.
  LMIC_setTxData2(1, mydata, strlen((char *)mydata), 0);
  SERIAL_PRINTF("%ld Packet queued\n", txFrameCounter);
}